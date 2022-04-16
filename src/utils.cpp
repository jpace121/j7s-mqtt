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

#include <cpr/cpr.h>
#include <nlohmann/json.hpp>

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

std::vector<std::string> stringSplit(const std::string &input, char delimiter)
{
    std::vector<std::string> toReturn;
    std::stringstream stream(input);

    std::string tmpString;
    while (std::getline(stream, tmpString, delimiter))
    {
        toReturn.push_back(tmpString);
    }

    return toReturn;
}

std::string fetchToken(const std::string& clientCert,
                       const std::string& clientKey,
                       const std::string& username,
                       const std::string& oidcAuthority,
                       const std::string& oidcClient)
{
    using json = nlohmann::json;

    cpr::SslOptions sslOpts = cpr::Ssl(cpr::ssl::CertFile{clientCert.c_str()}, cpr::ssl::KeyFile{clientKey.c_str()});

    cpr::Response well_known = cpr::Get(cpr::Url{oidcAuthority + ".well-known/openid-configuration"}, sslOpts);
    if(well_known.status_code == 404)
    {
        std::stringstream ss;
        ss << "Could not find token end point. Authority: " << oidcAuthority << "\n"
           << "(Hint: That should end in a slash, but this program doesn't confirm that...)";
        throw std::runtime_error(ss.str());
    }
    const auto json_well_known = json::parse(well_known.text);
    const std::string token_url = json_well_known["token_endpoint"];

    cpr::Payload data({{"grant_type", "password"}, {"client_id", oidcClient.c_str()}, {"username", username.c_str()}});
    cpr::Response token_resp = cpr::Post(cpr::Url{token_url}, sslOpts, data);
    if(token_resp.status_code == 404)
    {
        throw std::runtime_error("Could not get token.");
    }
    const auto json_token = json::parse(token_resp.text);

    return json_token["access_token"];
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
