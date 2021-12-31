#pragma once
#include <boost/asio.hpp>
#include <chrono>
#include <string>
#include <optional>
#include "conduit/mc_output_stream.hpp"

namespace conduit {
    namespace asio = boost::asio;
    asio::awaitable<std::optional<std::vector<unsigned char>>> ping_mc_server(asio::ip::address address, unsigned short port, const std::chrono::milliseconds& timeout);
    asio::awaitable<std::optional<std::string>> parse_mc_response(asio::ip::address address, unsigned short port, unsigned char *buf, size_t size);
    asio::awaitable<bool> ping_mc_server_with_parse(asio::ip::address address, unsigned short port, const std::chrono::milliseconds& timeout);
}