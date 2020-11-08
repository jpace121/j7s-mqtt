// Copyright 2020 James Pace
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#include <mqtt/async_client.h>
#include <argparse/argparse.hpp>
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <nlohmann/json.hpp>
#include <optional>
#include <sstream>
#include <string>
#include <thread>

namespace json = nlohmann;

std::optional<std::string> getEnv(const std::string & key)
{
    const char * pointer = std::getenv(key.c_str());

    if (pointer)
    {
        return std::string(pointer);
    }
    return std::nullopt;
}

void validate_color(const std::string & color)
{
    const std::array<std::string, 7> valid_colors = {"aqua", "red",   "lime", "green",
                                                     "blue", "white", "off"};
    for (const auto & valid_option : valid_colors)
    {
        if (color == valid_option)
        {
            return;
        }
    }

    // Given an invalid string.
    // Build a list of the valid ones to make the warning message more friendly.
    std::stringstream error_message;
    error_message << "Invalid color option. Read: " << color << " Valid Options: { ";
    for (const auto & valid_option : valid_colors)
    {
        error_message << valid_option << " ";
    }
    error_message << "}";

    throw std::runtime_error(error_message.str());
}

int main(int argc, char * argv[])
{
    argparse::ArgumentParser program("j7s-pub");
    // clang-format off
    program.add_argument("-c", "--color")
        .required()
        .help("Color to set controlled LED.");
    program.add_argument("-i", "--index")
        .required()
        .help("LED index this publisher controls.")
        .action([](const std::string & value) { return std::stoi(value); });
    program.add_argument("-f", "--frequency")
        .default_value(1.0)
        .help("Frequency for sin wave.")
        .action([](const std::string & value) { return std::stod(value); });
    program.add_argument("-d", "--display-frequency")
        .default_value(10.0)
        .help("Frequency to publish a new decision.")
        .action([](const std::string & value) { return std::stod(value); });
    // clang-format on
    try
    {
        program.parse_args(argc, argv);

        validate_color(program.get<std::string>("--color"));
    }
    catch (const std::runtime_error & err)
    {
        std::cout << err.what() << std::endl;
        std::cout << program;
        exit(0);
    }

    const std::string brokerAddr = getEnv("J7S_BROKER").value_or("localhost");

    mqtt::async_client client(brokerAddr, "");
    client.connect()->wait();

    json::json message;
    message["brightness"] = 0.0;
    message["index"] = program.get<int>("--index");
    message["color"] = program.get<std::string>("--color");

    mqtt::topic top(client, "led_state", 1);

    double time = 0.0;
    const auto sleep_time = 1.0 / program.get<double>("--display-frequency");
    const auto sleep_duration = std::chrono::milliseconds(static_cast<int>(sleep_time * 1e3));
    const auto freq = program.get<double>("--frequency");
    while (true)
    {
        message["brightness"] = sin(2 * M_PI * time * freq);
        top.publish(message.dump());
        std::this_thread::sleep_for(sleep_duration);
        time += sleep_time;
    }
    return 0;
}
