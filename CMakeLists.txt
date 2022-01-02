cmake_minimum_required(VERSION 3.0.0)
project(conduit VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 20)

find_package(Boost REQUIRED thread)

add_executable(conduit 
                        src/main.cpp 
                        src/net_parser.cpp 
                        src/mc_ping.cpp
                        src/mc_output_stream.cpp
                        src/mc_input_stream.cpp)
                        
target_include_directories(conduit PRIVATE include Boost::headers)
target_link_libraries(conduit PRIVATE Boost::thread)
