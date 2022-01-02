#include "conduit/mc_output_stream.hpp"
#include <boost/asio/ip/address.hpp>
#include <algorithm>

namespace conduit {
    namespace asio = boost::asio;
    
    void MinecraftOutputStream::write_varint(int value) {
        while (value != 0) {
            unsigned char byte = static_cast<unsigned char>(value & 0b01111111);
            value >>= 7;
            if (value != 0)
                byte |= 0b10000000;
            buffer.push_back(byte);
        }
    }

    void MinecraftOutputStream::write_string(const std::string& value) {
        write_varint(value.length());
        std::for_each(value.begin(), value.end(), [this](char c){ buffer.push_back(c); });
    }

    void MinecraftOutputStream::write_unsigned_short(unsigned short value) {
        buffer.push_back(value & 0b11111111);
        buffer.push_back(value >> 8);
    }

    void MinecraftOutputStream::write_handshake(const asio::ip::address& address, unsigned short port) {
        buffer.push_back(0x00);
        buffer.push_back(0x47);
        write_string(address.to_string());
        write_unsigned_short(port);
        buffer.push_back(0x01);
        
        prefix_length();
        buffer.push_back(0x01);
        buffer.push_back(0x00);
    }

    void MinecraftOutputStream::prefix_length() {
        MinecraftOutputStream buffer_length;
        buffer_length.write_varint(buffer.size());
        buffer.insert(buffer.begin(), buffer_length.get_buffer().begin(), buffer_length.get_buffer().end());
    }
}