# conduit-cpp
conduit-cpp is a fast Minecraft server scanner.

## Motivation
The goal was to get more accustomed to C++ and asio.
  
## Help usage
```
Usage: Conduit 2.0 [options] target 

Positional arguments:
target           	The target range

Optional arguments:
-h --help        	shows help message and exits [default: false]
-v --version     	prints version information and exits [default: false]
-p --ports       	The port range [default: "25565"]
-t --timeout     	Timeout in milliseconds for connecting to and reading from servers [default: 1000]
-s --scan-size   	The maximum # of scans occuring at a time [default: 256]
-b --buffer-size 	The size of buffer used to read from TCP streams [default: 1024]
-i --ipv6        	IPv6 mode [default: false]
-o --output      	Output to file [default: ""]
-v --verbose     	Verbose mode [default: false]
```
