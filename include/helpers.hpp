#pragma once
#include <vector>
#include <string>

namespace conduit {

    std::vector<std::byte> write_varint(int value);
    std::vector<std::byte> write_string(const std::string& str);

    int read_varint(std::byte *buffer);
    std::string read_string(std::byte *buffer);

    int calculate_varint_length(int value);
};