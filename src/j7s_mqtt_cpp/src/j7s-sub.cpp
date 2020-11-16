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

#include <iostream>

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

    {
        _blinkt.setPixel(index, pixel);
        _blinkt.display();
    }

    return;
}

int main(int, char **)
{
    const std::string brokerAddr = getEnv("J7S_BROKER").value_or("localhost");

    // Single blinkt object.
    blinkt_interface::Blinkt blinkt;

    mqtt::async_client client(brokerAddr, "");
    J7sSubCallback callback(client, blinkt);
    client.set_callback(callback);

    client.connect();

    while(true)
    {
    }

    return 0;
}
