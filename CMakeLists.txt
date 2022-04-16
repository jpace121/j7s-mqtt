cmake_minimum_required(VERSION 3.5)
project(j7s_mqtt)

include(external-deps.cmake)

set(CMAKE_CXX_STANDARD 17)
add_compile_options(-Wall -Wextra -Wpedantic)

find_package(PahoMqttCpp REQUIRED)
find_package(Threads REQUIRED)

add_library(j7s-mqtt-utils SHARED src/utils.cpp)
target_include_directories(j7s-mqtt-utils PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>)
target_link_libraries(j7s-mqtt-utils
  PRIVATE blinkt nlohmann_json::nlohmann_json cpr)

add_executable(j7s-pub src/j7s-pub.cpp)
target_include_directories(j7s-pub PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>)
target_link_libraries(j7s-pub
  PRIVATE blinkt PahoMqttCpp::paho-mqttpp3 argparse::argparse nlohmann_json::nlohmann_json fmt j7s-mqtt-utils)

add_executable(j7s-sub src/j7s-sub.cpp)
target_include_directories(j7s-sub PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>)
target_link_libraries(j7s-sub
  PRIVATE blinkt PahoMqttCpp::paho-mqttpp3 argparse::argparse nlohmann_json::nlohmann_json j7s-mqtt-utils)

install(TARGETS j7s-pub j7s-sub j7s-mqtt-utils
  EXPORT export_${PROJECT_NAME}
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
)
