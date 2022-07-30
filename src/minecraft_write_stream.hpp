#pragma once
#include "minecraft_types.hpp"
#include "asio/ip/tcp.hpp"
#include <vector>
#include <string_view>

namespace conduit {
    class minecraft_write_stream {
    public:
        minecraft_write_stream(std::vector<mc_byte>* buffer);
        void write_byte(mc_byte byte);
        void write_varint(int value);
        void write_string(const std::string& str);
        void write_unsigned_short(unsigned short value);
        void prefix_length();
    private:
        std::vector<mc_byte>* m_buffer;
    };
}