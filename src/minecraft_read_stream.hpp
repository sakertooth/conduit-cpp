#pragma once
#include "minecraft_types.hpp"
#include "asio/ip/tcp.hpp"
#include <vector>

namespace conduit {
    class minecraft_read_stream {
    public:
        minecraft_read_stream(std::vector<mc_byte>* buffer);

        int read_varint();
        std::string read_string();   
        void reset_position();
    private:
        std::vector<mc_byte>* m_buffer;
        int m_position = 0;
    };
}