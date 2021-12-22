cmake_minimum_required(VERSION 3.0.0)
project(conduit VERSION 0.1.0)

include(CTest)
enable_testing()

set(CXX_STANDARD 17)
find_package(Boost REQUIRED system)

add_executable(conduit src/main.cpp src/scanworker.cpp src/mcstreams.cpp)
target_include_directories(conduit PRIVATE include)
target_link_libraries(conduit PRIVATE Boost::system)

if(UNIX)
    target_link_libraries(conduit PRIVATE pthread)
endif()

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
