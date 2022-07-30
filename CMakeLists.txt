cmake_minimum_required(VERSION 3.0.0)
project(conduit VERSION 2.0)

set(CMAKE_CXX_STANDARD 20)

add_executable(conduit 
    src/main.cpp 
    src/net_parser.cpp
    src/minecraft_ping_task.cpp
    src/minecraft_write_stream.cpp
    src/minecraft_read_stream.cpp
    src/varint_helper.cpp
    src/conduit_configuration.cpp
    src/conduit_logger.cpp)

target_include_directories(conduit PRIVATE 
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/3rdparty/argparse/include
    ${CMAKE_SOURCE_DIR}/3rdparty/asio/asio/include)

#Download argparse library
file(DOWNLOAD
    https://raw.githubusercontent.com/p-ranav/argparse/master/include/argparse/argparse.hpp
    ${CMAKE_SOURCE_DIR}/include/argparse/argparse.hpp)

#Download nlohmann's json library
file(DOWNLOAD
    https://raw.githubusercontent.com/nlohmann/json/develop/single_include/nlohmann/json.hpp 
    ${CMAKE_SOURCE_DIR}/include/nlohmann/json.hpp)