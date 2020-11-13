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

#include <thread>
#include <mutex>

using json = nlohmann::json;

class J7sSubCallback : public mqtt::callback
{
public:
    J7sSubCallback(mqtt::async_client& client,
                   blinkt_interface::Blinkt& blinkt,
                   std::mutex& blinkt_mutex);

private:
    mqtt::async_client& _client;
    blinkt_interface::Blinkt& _blinkt;
    std::mutex& _blinkt_mutex;

    void connected(const std::string& cause) override;
    void message_arrived(mqtt::const_message_ptr msg) override;
};

J7sSubCallback::J7sSubCallback(
    mqtt::async_client & client, blinkt_interface::Blinkt & blinkt, std::mutex & blinkt_mutex) :
    _client(client),
    _blinkt(blinkt),
    _blinkt_mutex(blinkt_mutex){}

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
        std::lock_guard<std::mutex> lock(_blinkt_mutex);
        _blinkt.setPixel(index, pixel);
    }

    return;
}

void displayFunc(const blinkt_interface::Blinkt & blinkt, std::mutex & blinkt_mutex, int sleep_time_ms)
{
    while(true)
    {
        {
            std::lock_guard<std::mutex> lock(blinkt_mutex);
            blinkt.display();
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(sleep_time_ms));
    }
}

int main(int, char **)
{
    const std::string brokerAddr = getEnv("J7S_BROKER").value_or("localhost");

    const double display_rate = std::stod(getEnv("J7S_DISPLAY_RATE").value_or("100"));
    const int sleep_time_ms = static_cast<int>(floor(1.0 / display_rate) * 1e3);

    // Single blinkt object.
    blinkt_interface::Blinkt blinkt;
    std::mutex blinkt_mutex;

    std::thread display_thread(displayFunc, std::cref(blinkt), std::ref(blinkt_mutex), sleep_time_ms);

    mqtt::async_client client(brokerAddr, "");
    J7sSubCallback callback(client, blinkt, blinkt_mutex);
    client.set_callback(callback);

    client.connect();

    while(true)
    {
    }

    return 0;
}
