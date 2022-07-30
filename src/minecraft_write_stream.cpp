#include "minecraft_write_stream.hpp"

namespace conduit {
    constexpr int segment_bits = 0x7F;
    constexpr int continue_bit = 0x80;

    minecraft_write_stream::minecraft_write_stream(std::vector<mc_byte>* buffer) : m_buffer(buffer) {}

    void minecraft_write_stream::write_byte(mc_byte byte) {
        m_buffer->push_back(byte);
    }

    void minecraft_write_stream::write_varint(int value) {
        while (true) {
            if ((value & ~segment_bits) == 0) {
                write_byte(value);
                return;
            }

            write_byte((value & segment_bits) | continue_bit);
            value = static_cast<int>(static_cast<unsigned int>(value) >> 7);
        }
    }

    void minecraft_write_stream::write_string(const std::string& str) {
        write_varint(str.length());
        m_buffer->insert(m_buffer->end(), str.begin(), str.end());
    }

    void minecraft_write_stream::write_unsigned_short(unsigned short value) {
        write_byte(static_cast<mc_byte>(value >> 8));
        write_byte(static_cast<mc_byte>(value));
    }

    void minecraft_write_stream::prefix_length() {
        std::vector<mc_byte> write_stream_buf;
        minecraft_write_stream write_stream {&write_stream_buf};
        write_stream.write_varint(m_buffer->size());
        m_buffer->insert(m_buffer->begin(), write_stream_buf.begin(), write_stream_buf.end());
    }
}