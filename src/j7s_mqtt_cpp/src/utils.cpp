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

#include <array>
#include <sstream>

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

blinkt_interface::Pixel stringToPixel(const std::string& color, double brightness)
{
    if(color == "red")
    {
        return blinkt_interface::color::red(brightness);
    }
    else if(color == "lime")
    {
        return blinkt_interface::color::lime(brightness);
    }
    else if(color == "green")
    {
        return blinkt_interface::color::green(brightness);
    }
    else if(color == "blue")
    {
        return blinkt_interface::color::blue(brightness);
    }
    else if(color == "white")
    {
        return blinkt_interface::color::white(brightness);
    }
    else if(color == "aqua")
    {
        return blinkt_interface::color::aqua(brightness);
    }
    else if(color == "off")
    {
        return blinkt_interface::color::off(brightness);
    }
    else
    {
        throw std::logic_error("Invalid color provided.");
    }
}
