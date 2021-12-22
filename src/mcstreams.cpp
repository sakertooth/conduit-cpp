#include "mcstreams.hpp"
#include <algorithm>

void conduit::MinecraftOutputStream::write_byte(unsigned char byte) {
    buffer.push_back(byte);
}

void conduit::MinecraftOutputStream::write_varint(int value) {
    while (true) {
        if ((value & ~0x7F) == 0) {
          buffer.push_back(static_cast<unsigned char>(value));
          return;
        }

        buffer.push_back((value & 0x7F) | 0x80);
        value = static_cast<unsigned int>(value) >> 7;
    }
}

void conduit::MinecraftOutputStream::write_string(const std::string& str) {
    write_varint(str.length());
    buffer.insert(buffer.end(), str.begin(), str.end());
}

void conduit::MinecraftOutputStream::write_unsigned_short(unsigned short value) {
    buffer.push_back(static_cast<unsigned char>(value << 8));
    buffer.push_back(static_cast<unsigned char>(value >> 8));
}

int conduit::MinecraftInputStream::read_varint() {
    
}