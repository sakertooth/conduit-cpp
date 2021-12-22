#pragma once
#include <vector>

namespace conduit {
    std::vector<unsigned char> encode_varint(int value);
    int decode_varint(std::vector<unsigned char>& bytes);
}