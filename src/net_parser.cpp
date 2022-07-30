#include <iostream>
#include <sstream>
#include "net_parser.hpp"

namespace conduit {
    std::variant<asio::ip::network_v4, asio::ip::network_v6> parse_targetrange(const std::string& str, bool isIPv6) {
        if (!isIPv6) {
            return asio::ip::make_network_v4(str);
        }
        else {
            return asio::ip::make_network_v6(str);
        }
    }

    std::vector<unsigned short> parse_portrange(const std::string& str) {
        std::vector<unsigned short> ports;
        std::stringstream ss(str);
        std::string token;

        while (std::getline(ss, token, ',')) {
            std::size_t pos = token.find('-');
            if (pos != std::string::npos) {
                auto min = std::stoi(token.substr(0, pos));
                auto max = std::stoi(token.substr(pos + 1));
                
                for (int i = min; i <= max; i++) {
                    ports.push_back(i);
                }
            }
            else {
                ports.push_back(std::stoi(token));
            }
        }

        return ports;
    }
}

