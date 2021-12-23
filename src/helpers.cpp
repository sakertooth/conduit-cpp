#include "helpers.hpp"
#include <algorithm>
#include <stdexcept>
#include <iostream>
#include <cstddef>

std::vector<std::byte> conduit::write_varint(int value) {
    std::vector<std::byte> buffer;

    while (true) {
        if ((value & ~0x7F) == 0) {
          buffer.push_back(static_cast<std::byte>(value));
          return buffer;
        }

        buffer.push_back(static_cast<std::byte>((value & 0x7F) | 0x80));
        value = static_cast<unsigned int>(value) >> 7;
    }

    return buffer;
}

std::vector<std::byte> conduit::write_string(const std::string& str) {
    std::vector<std::byte> buffer;
    
    auto str_length_varint = write_varint(str.length());
    buffer.insert(buffer.end(), str_length_varint.begin(), str_length_varint.end());

    std::vector<std::byte> str_buffer(str.size());
    std::transform(str.begin(), str.end(), str_buffer.begin(), [](char c){ return static_cast<std::byte>(c); });
    buffer.insert(buffer.end(), str_buffer.begin(), str_buffer.end());

    return buffer;
}

int conduit::read_varint(std::byte *buffer) {
    int value = 0;
    int length = 0;

    int buffer_pos = 0;
    std::byte currentByte;

    while (true) {
        currentByte = *(buffer + buffer_pos);
        value |= static_cast<int>((currentByte & static_cast<std::byte>(0x7F)) << (length * 7));
        
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

std::string conduit::read_string(std::byte *buffer) {
    int str_length = read_varint(buffer);
    buffer += calculate_varint_length(str_length);

    std::string str;
    std::transform(buffer, buffer + str_length, str.begin(), [](std::byte byte) { return static_cast<char>(byte); });
    return str;
}

int conduit::calculate_varint_length(int value) {
    int length = 0;
    do {
        value >>= 7;
        ++length;
    }
    while (value != 0);
    return length;
}