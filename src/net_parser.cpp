#include <iostream>
#include <sstream>
#include <boost/asio/ip/address.hpp>
#include <boost/asio/ip/network_v4.hpp>
#include <boost/system/error_code.hpp>
#include "conduit/net_parser.hpp"

using boost::system::error_code;
using boost::asio::ip::network_v4;

network_v4 conduit::parse_targetrange(const std::string& str) {
    //TODO: expand range types
    return boost::asio::ip::make_network_v4(str);
}

std::vector<unsigned short> conduit::parse_portrange(const std::string& str) {
    std::vector<unsigned short> ports;
    std::stringstream ss(str);
    std::string token;

    while (std::getline(ss, token, ',')) {
        try {
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
        catch (const std::invalid_argument& ex) {
            std::cout << "An error occured while parsing the port range\n";
        }
    }

    return ports;
}
