#pragma once
#include <vector>
#include <string>
#include <boost/asio/ip/address.hpp>

namespace conduit
{
    namespace asio = boost::asio;

    class MinecraftOutputStream {
    public:
        void write_varint(int value);
        void write_string(const std::string& value);
        void write_unsigned_short(unsigned short value);
        void write_handshake(const asio::ip::address& address, unsigned short port);
        void pack();
        std::vector<unsigned char>& get_buffer() { return buffer; };
    private:
        std::vector<unsigned char> buffer;
    };
}
