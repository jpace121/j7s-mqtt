cmake_minimum_required(VERSION 3.5)
project(j7s_mqtt_cpp)

set(CMAKE_CXX_STANDARD 17)
add_compile_options(-Wall -Wextra -Wpedantic)

find_package(ament_cmake REQUIRED)
find_package(blinkt_interface REQUIRED)
# Paho.
find_package(PahoMqttCpp REQUIRED)
find_package(Threads REQUIRED)
# argparse
find_package(argparse)
# JSON
find_package(nlohmann_json 3.9.1)

add_executable(j7s-pub src/j7s-pub.cpp)
target_include_directories(j7s-pub PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>)
target_link_libraries(j7s-pub
  PRIVATE blinkt_interface::blinkt PahoMqttCpp::paho-mqttpp3 argparse::argparse nlohmann_json::nlohmann_json)

install(TARGETS j7s-pub
  EXPORT ${PROJECT_NAME}
  DESTINATION lib/${PROJECT_NAME})

ament_package()
