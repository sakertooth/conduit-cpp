#include "minecraft_ping_task.hpp"
#include "minecraft_write_stream.hpp"
#include "minecraft_read_stream.hpp"
#include "asio/write.hpp"
#include "asio/read.hpp"
#include "varint_helper.hpp"
#include "nlohmann/json.hpp"
#include "conduit_configuration.hpp"
#include "conduit_logger.hpp"
#include <iostream>

namespace conduit {
    minecraft_ping_task::minecraft_ping_task(asio::io_context& io, const asio::ip::address& address, unsigned short port): 
        m_socket(io), m_endpoint(address, port),
        m_deadline(io), m_buffer_size(conduit_configuration::buffer_size()), 
        m_verbose(conduit_configuration::verbose()), m_timeout(conduit_configuration::timeout()) {}

    void minecraft_ping_task::start() {
        m_socket.async_connect(m_endpoint, std::bind(&minecraft_ping_task::handle_connect, this, std::placeholders::_1));
        m_deadline.expires_after(m_timeout);
        m_deadline.async_wait(std::bind(&minecraft_ping_task::handle_timeout, this, std::placeholders::_1));
    }

    void minecraft_ping_task::read_from_server() {
        auto buf = std::make_shared<std::vector<mc_byte>>(m_buffer_size);
        m_socket.async_read_some(asio::buffer(*buf), [this, buf](const asio::error_code& ec, size_t bytes_transferred) {
            m_deadline.cancel();

            if (!ec) {
                m_read_buffer.insert(m_read_buffer.end(), buf->begin(), buf->begin() + bytes_transferred);
                handle_read(ec, bytes_transferred);
            }
            else {
                if (m_verbose) {
                    conduit_logger::instance() << "Failed to read data from " << m_endpoint << ": " << ec.message() << '\n';
                }

                m_socket.close();
            }
        });

        m_deadline.expires_after(m_timeout);
        m_deadline.async_wait(std::bind(&minecraft_ping_task::handle_timeout, this, std::placeholders::_1));
    }

    void minecraft_ping_task::handle_connect(const asio::error_code& ec) {
        m_deadline.cancel();

        if (!ec) {
            std::vector<mc_byte> handshake_with_request;
            minecraft_write_stream write_stream {&handshake_with_request};
            write_stream.write_byte(0x00);
            write_stream.write_varint(47);
            write_stream.write_string(m_endpoint.address().to_string() + ":" + std::to_string(m_endpoint.port()));
            write_stream.write_unsigned_short(m_endpoint.port());
            write_stream.write_varint(1);
            write_stream.prefix_length();

            write_stream.write_byte(0x01);
            write_stream.write_varint(0x00);            
            
            asio::async_write(m_socket, 
                asio::buffer(handshake_with_request),
                std::bind(&minecraft_ping_task::handle_write, this, std::placeholders::_1, std::placeholders::_2)
            );
        }
        else {
            m_socket.close();
        }
    }

    void minecraft_ping_task::handle_write(const asio::error_code& ec, size_t) {
        if (!ec) {
            read_from_server();
        }
        else {
            m_socket.close();
        }
    }

    void minecraft_ping_task::handle_read(const asio::error_code& ec, size_t) {
        if (!ec) {
            int packet_length = 0;
            minecraft_read_stream read_stream {&m_read_buffer};
            try {
                packet_length = read_stream.read_varint();
            }
            catch (const std::out_of_range&) {
                read_from_server();
                return;
            }
            catch (const std::runtime_error& ex) {
                if (m_verbose) {
                    conduit_logger::instance() << "Failed reading VarInt from " << m_endpoint << ": " << ex.what() << '\n';
                }
                m_socket.close();
                return;
            }

            auto total_packet_length = calculate_varint_length(packet_length) + packet_length;
            if (m_read_buffer.size() < static_cast<size_t>(total_packet_length)) {
                auto bytes_remaining = total_packet_length - m_read_buffer.size();
                auto buf = std::make_shared<std::vector<mc_byte>>(bytes_remaining);
                asio::async_read(m_socket, asio::buffer(*buf), asio::transfer_exactly(bytes_remaining), [this, buf](const asio::error_code& ec, size_t bytes_transferred) {
                    if (!ec) {
                        m_read_buffer.insert(m_read_buffer.end(), buf->begin(), buf->end());
                        handle_read(ec, bytes_transferred);
                    }
                    else {
                        if (m_verbose) {
                            conduit_logger::instance() << "Failed to read from " << m_endpoint << ": " << ec.message() << '\n';
                        }
                        m_socket.close();
                    }
                });
                return;
            }

            //Skip Packet ID (0x00)
            read_stream.read_varint();

            process_response(read_stream.read_string());
        }
    }

    void minecraft_ping_task::handle_timeout(const asio::error_code& ec) {
        if (!ec) {
            m_socket.close();
        }
    }

    void minecraft_ping_task::process_response(const std::string& response) {
        nlohmann::json response_json;
        try {
            response_json = nlohmann::json::parse(response);
        }
        catch (nlohmann::json::parse_error& error) {
            if (m_verbose) {
                conduit_logger::instance() << "An error occurred while parsing the response from " << m_endpoint << '\n';
            }
            m_socket.close();
            return;
        }

        auto version_name = response_json["version"]["name"];
        auto players_max = response_json["players"]["max"];
        auto players_online = response_json["players"]["online"];

        auto description = response_json["description"];
        if (description.is_object()) {
            description = response_json["description"]["text"];
        }

        conduit_logger::instance() << m_endpoint << " [" << version_name << "] " << "(" << players_online << "/" << players_max << ") " << description << '\n'; 
        m_succeeded = true;
        m_socket.close();
    }

    bool minecraft_ping_task::succeeded() const {
        return m_succeeded;
    }
}