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

    // Get connection settings from environment variables.
    const auto clientCertOpt = getEnv("J7S_CLIENT_CERT");
    const auto clientKeyOpt = getEnv("J7S_CLIENT_KEY");
    if((not clientCertOpt) or (not clientKeyOpt))
    {
        throw std::runtime_error("Missing J7S_CLIENT_CERT or J7S_CLIENT_KEY.");
    }
    const auto clientCert = std::filesystem::absolute(clientCertOpt.value());
    const auto clientKey = std::filesystem::absolute(clientKeyOpt.value());

    const auto brokerAddr = getEnv("J7S_BROKER").value_or("ssl://mqtt.jpace121.net:8883");
    const auto username = getEnv("J7S_USERNAME").value_or(getEnv("USER").value_or("j7s"));
    const auto oidcAuthority = getEnv("J7S_AUTHORITY").value_or("https://auth.jpace121.net/realms/jpace121-services/");
    const auto oidcClient = getEnv("J7S_OIDC_CLIENT").value_or("mqtt");

    // Get token from Keycloak.
    const auto token = fetchToken(clientCert, clientKey, username, oidcAuthority, oidcClient);

    // Now we can build the connection object for the broker.
    auto sslopts = mqtt::ssl_options_builder()
        .key_store(clientCert.c_str())
        .private_key(clientKey.c_str())
        .finalize();
    auto connopts = mqtt::connect_options_builder()
                    .user_name(username)
                    .password(token)
                    .ssl(sslopts)
                    .finalize();

    // Now we can connect.
    mqtt::async_client client(brokerAddr, username + "-pub");
    client.connect(connopts)->wait();

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
