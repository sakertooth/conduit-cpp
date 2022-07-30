#include "minecraft_read_stream.hpp"
#include <iostream>

namespace conduit {
    constexpr int segment_bits = 0x7F;
    constexpr int continue_bit = 0x80;

    minecraft_read_stream::minecraft_read_stream(std::vector<mc_byte>* buffer) : m_buffer(buffer) {}

    int minecraft_read_stream::read_varint() {
        int value = 0;
        int position = 0;
        mc_byte current_byte;

        while (true) {
            current_byte = m_buffer->at(m_position++);
            value |= (current_byte & segment_bits) << position;

            if ((current_byte & continue_bit) == 0) break;

            position += 7;

            if (position >= 32) throw std::runtime_error{"VarInt is too big"};
        }

        return value;
    }

    std::string minecraft_read_stream::read_string() {
        int str_length = read_varint();
        std::string result {m_buffer->begin() + m_position, m_buffer->begin() + m_position + str_length};
        m_position += str_length;
        return result;
    }

    void minecraft_read_stream::reset_position() {
        m_position = 0;
    }
}