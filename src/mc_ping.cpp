#include "conduit/mc_ping.hpp"
#include "conduit/mc_output_stream.hpp"
#include "conduit/mc_input_stream.hpp"
#include "nlohmann/json.hpp"
#include <boost/asio.hpp>
#include <boost/asio/experimental/awaitable_operators.hpp>
#include <iostream>
#include "termcolor/termcolor.hpp"
    
#define TERMCOLOR_USE_ANSI_ESCAPE_SEQUENCES

namespace conduit {
    namespace asio = boost::asio;
    using namespace asio::experimental::awaitable_operators;
    using tcp = asio::ip::tcp;
    
    asio::awaitable<std::optional<std::vector<unsigned char>>> ping_mc_server(asio::ip::address address, unsigned short port, const std::chrono::milliseconds& timeout) {
        asio::ip::tcp::socket socket(co_await asio::this_coro::executor);
        asio::steady_timer timer{co_await asio::this_coro::executor};

        timer.expires_after(timeout);
        auto connect_result = co_await (
            socket.async_connect(tcp::endpoint(address, port), asio::use_awaitable) || 
            timer.async_wait(asio::use_awaitable));

        if (connect_result.valueless_by_exception() || connect_result.index() == 1) {
            socket.close();
            co_return std::nullopt;
        }

        conduit::MinecraftOutputStream request_stream;
        request_stream.write_handshake(address, port);

        timer.expires_after(timeout);
        auto write_result = co_await (
            asio::async_write(socket, asio::buffer(request_stream.get_buffer()), asio::transfer_all(), asio::use_awaitable) ||
            timer.async_wait(asio::use_awaitable));

        if (write_result.valueless_by_exception() || write_result.index() == 1) {
            socket.close();
            co_return std::nullopt;
        }

        int body_length = 0;
        int shift = 0;
        while (true) {
            unsigned char byte;
            timer.expires_after(timeout);
            auto read_byte_result = co_await (
                asio::async_read(socket, asio::buffer(&byte, 1), asio::transfer_exactly(1), asio::use_awaitable) ||
                timer.async_wait(asio::use_awaitable));

            if (read_byte_result.valueless_by_exception() || read_byte_result.index() == 1) {
                socket.close();
                co_return std::nullopt;
            }

            body_length |= (byte & 0b01111111) << shift;
            if ((byte & 0b10000000) == 0) {
                break;
            }
            shift += 7;
        }

        std::vector<unsigned char> body(body_length);

        timer.expires_after(timeout);
        auto read_body_result = co_await (
            asio::async_read(socket, asio::buffer(body), asio::transfer_exactly(body_length), asio::use_awaitable) ||
            timer.async_wait(asio::use_awaitable));

        if (read_body_result.valueless_by_exception() || read_body_result.index() == 1) {
            socket.close();
            co_return std::nullopt;
        }

        socket.close();
        co_return body;
    }

    asio::awaitable<std::optional<std::string>> parse_mc_response(asio::ip::address address, unsigned short port, unsigned char *buf, size_t size) {
        conduit::MinecraftInputStream response_stream(buf);
        if (response_stream.read_varint() != 0x00) {
            co_return std::nullopt;
        }
        
        try {
            auto json = nlohmann::json::parse(response_stream.read_string());
            std::stringstream ss;

            ss << address.to_string() + ":" + std::to_string(port) + " ";
            ss << "[" << json["version"]["name"] << "] ";
            ss << "(" << json["players"]["online"] << "/" << json["players"]["max"] << ") ";

            if (json["description"].is_string())
                ss << json["description"];
            else if (json["description"].is_object())
                ss << json["description"]["text"];
            co_return ss.str();
        }
        catch (const std::exception& ex) {
            co_return std::nullopt;
        }
    }

    asio::awaitable<bool> ping_mc_server_with_parse(asio::ip::address address, unsigned short port, const std::chrono::milliseconds& timeout) {
        auto response_buf = co_await ping_mc_server(address, port, timeout);
        if (!response_buf.has_value())
            co_return false;
            
        auto parsed_response = co_await parse_mc_response(address, port, response_buf->data(), response_buf->size());
        if (!parsed_response.has_value())
            co_return false;

        std::cout << *parsed_response << '\n';
        co_return true;
    }
}