#pragma once
#include "asio/ip/address.hpp"
#include "asio/ip/tcp.hpp"
#include "asio/steady_timer.hpp"
#include "minecraft_types.hpp"

namespace conduit {
    class minecraft_ping_task {
    public:
        minecraft_ping_task(asio::io_context& io, const asio::ip::address& address, unsigned short port);
        minecraft_ping_task(const minecraft_ping_task& other) = delete;
        minecraft_ping_task(minecraft_ping_task&& other) = default;

        minecraft_ping_task& operator=(const minecraft_ping_task& other) = delete;
        minecraft_ping_task& operator=(minecraft_ping_task&& other) = default;

        void start();
        void read_from_server();

        void handle_connect(const asio::error_code& ec);
        void handle_write(const asio::error_code& ec, size_t bytes_transferred);
        void handle_read(const asio::error_code& ec, size_t bytes_transferred);
        void handle_timeout(const asio::error_code& ec);
        void process_response(const std::string& response);
        bool succeeded() const;

    private:
        asio::ip::tcp::socket m_socket;
        asio::ip::tcp::endpoint m_endpoint;
        asio::steady_timer m_deadline;
        std::vector<mc_byte> m_read_buffer;
        int m_buffer_size = 0;
        bool m_verbose = false;
        bool m_succeeded = false;
        std::chrono::milliseconds m_timeout;
    };
}