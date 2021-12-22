#include "varint.hpp"
#include <stdexcept>

std::vector<unsigned char> conduit::encode_varint(int value) {
    std::vector<unsigned char> bytes;

    while (true) {
        if ((value & ~0x7F) == 0) {
          bytes.push_back(value);
          return bytes;
        }


        bytes.push_back((value & 0x7F) | 0x80);
        value = static_cast<unsigned int>(value) >> 7;
    }

    return bytes;
}

int conduit::decode_varint(std::vector<unsigned char>& bytes) {
    int value = 0;
    int length = 0;
    unsigned char current_byte;

    while (true) {
        current_byte = bytes.front();
        bytes.erase(bytes.begin());
        
        value |= (current_byte & 0x7F) << (length * 7);
        
        length += 1;
        if (length > 5) {
            throw std::out_of_range("VarInt is too big");
        }

        if ((value & 0x80) != 0x80) {
            break;
        }
    }
    return value;
}