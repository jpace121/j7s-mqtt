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

#include <j7s_mqtt_cpp/utils.hpp>

#include <mqtt/async_client.h>
#include <argparse/argparse.hpp>
#include <nlohmann/json.hpp>

#include <chrono>
#include <cmath>
#include <iostream>
#include <string>
#include <thread>

#include <iostream>

namespace json = nlohmann;

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
        std::cout << "Publish brightness: " << message["brightness"] << std::endl;
        top.publish(message.dump());
        std::this_thread::sleep_for(sleep_duration);
        time += sleep_time;
    }
    return 0;
}
