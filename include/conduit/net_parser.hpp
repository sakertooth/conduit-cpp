#pragma once
#include <vector>
#include <boost/asio/ip/network_v4.hpp>

namespace conduit {
    using boost::asio::ip::network_v4;

    network_v4 parse_targetrange(const std::string& str);
    std::vector<unsigned short> parse_portrange(const std::string& str);
};
