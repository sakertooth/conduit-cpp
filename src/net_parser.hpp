#pragma once
#include <vector>
#include <variant>
#include "asio/ip/network_v4.hpp"
#include "asio/ip/network_v6.hpp"

namespace conduit {
    std::variant<asio::ip::network_v4, asio::ip::network_v6> parse_targetrange(const std::string& str, bool isIPv6);
    std::vector<unsigned short> parse_portrange(const std::string& str);
};
