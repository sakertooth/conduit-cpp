#pragma once
#include <vector>
#include <string>

namespace conduit {
    class MinecraftOutputStream {
        public:
            void write_byte(unsigned char byte);
            void write_varint(int value);
            void write_string(const std::string& str);
            void write_unsigned_short(unsigned short value);

            constexpr std::vector<unsigned char>& get_buffer() const;
        private:
            std::vector<unsigned char> buffer;
    };

    class MinecraftInputStream {
        public:
            MinecraftInputStream(std::vector<unsigned char>& buffer);

            int read_varint();
            std::string read_string();
            void resize(std::size_t size);

            constexpr std::vector<unsigned char>& get_buffer() const;
        private:
            std::vector<unsigned char>& bufferRef;
            int pos;
    }
};