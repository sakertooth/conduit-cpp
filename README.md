# conduit-cpp
conduit-cpp is a fast, C++20 coroutine based Minecraft server scanner.

## Motivation
This project was to get an idea for C++20, more specifically its coroutine feature.  
The goal of learning more about C++ was also assumed when making this project.

## Requirements for building
  * [CMake](https://github.com/Kitware/CMake)
  * [Boost.Asio](https://www.boost.org/doc/libs/develop/doc/html/boost_asio.html)
  
## Help usage
```
Usage: Conduit [options] target 

Positional arguments:
target       	the target range

Optional arguments:
-h --help    	shows help message and exits [default: false]
-v --version 	prints version information and exits [default: false]
-p --ports   	the port range [default: "25565"]
-t --timeout 	timeout in milliseconds for connecting, writing, and reading [default: 1000]
-s --size    	the maximum # of scans occuring at a time [default: 256]
```
