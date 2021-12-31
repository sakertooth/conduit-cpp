#include "conduit/mc_input_stream.hpp"

conduit::MinecraftInputStream::MinecraftInputStream(unsigned char *buffer) : buffer(buffer), pos(0) {}

int conduit::MinecraftInputStream::read_varint() {
    int result = 0;
    int shift = 0;
    while (true) {
        unsigned char byte = *(buffer + pos++);
        result |= (byte & 0b01111111) << shift;
        
        if ((byte & 0b10000000) == 0)
            break;
        shift += 7;
    }

    return result;
}

std::string conduit::MinecraftInputStream::read_string() {
    long unsigned int length = read_varint();
    auto str = std::string{reinterpret_cast<char*>(buffer + pos), length};
    pos += length;
    return str;
}

