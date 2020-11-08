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

#include <optional>
#include <string>

// Try to get an environment variable, either return an optional
// with the value of the environment variable, or nullopt if the
// environment variable isn't set.
std::optional<std::string> getEnv(const std::string & key);

// Verify color is one the valid colors Blinkt has preprogrammed.
void validate_color(const std::string & color);
