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

#include <blinkt_interface/blinkt.hpp>

#include <mqtt/async_client.h>
#include <nlohmann/json.hpp>

#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <thread>
#include <chrono>

using json = nlohmann::json;

class J7sSubCallback : public mqtt::callback
{
public:
    J7sSubCallback(mqtt::async_client& client,
                   blinkt_interface::Blinkt& blinkt);

private:
    mqtt::async_client& _client;
    blinkt_interface::Blinkt& _blinkt;

    void connected(const std::string& cause) override;
    void message_arrived(mqtt::const_message_ptr msg) override;
};

J7sSubCallback::J7sSubCallback(
    mqtt::async_client & client, blinkt_interface::Blinkt & blinkt) :
    _client(client),
    _blinkt(blinkt){}

void J7sSubCallback::connected(const std::string&)
{
    _client.subscribe("led_state", 0);
}

void J7sSubCallback::message_arrived(mqtt::const_message_ptr msg)
{
    if(msg->get_topic() != "led_state")
    {
        return;
    }

    if(not json::basic_json::accept(msg->get_payload_str()))
    {
        return;
    }

    const auto payload = json::parse(msg->get_payload_str());

    const auto index = payload["index"];
    const auto pixel = stringToPixel(payload["color"], payload["brightness"]);

    _blinkt.setPixel(index, pixel);
    _blinkt.display();

    return;
}


int main(int, char **)
{
    // Get settings from environment variables.
    const auto clientCertOpt = getEnv("J7S_CLIENT_CERT");
    const auto clientKeyOpt = getEnv("J7S_CLIENT_KEY");
    const auto userNameOpt = getEnv("J7S_USERNAME");

    if((not clientCertOpt) or (not clientKeyOpt))
    {
        throw std::runtime_error("Missing J7S_CLIENT_CERT or J7S_CLIENT_KEY.");
    }
    const auto clientCert = std::filesystem::absolute(clientCertOpt.value());
    const auto clientKey = std::filesystem::absolute(clientKeyOpt.value());

    if(not userNameOpt)
    {
        throw std::runtime_error("Missing J7S_USERNAME.");
    }
    const auto username = userNameOpt.value();

    const auto brokerAddr = getEnv("J7S_BROKER").value_or("ssl://mqtt.jpace121.net:8883");
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

    // Single blinkt object.
    blinkt_interface::Blinkt blinkt;

    // Build the client.
    mqtt::async_client client(brokerAddr, username);
    // ... and the client class.
    J7sSubCallback callback(client, blinkt);
    client.set_callback(callback);

    // Connect.
    client.connect(connopts);

    while(true){}

    return 0;
}
