#pragma once
#include <string>

namespace conduit {
    class MinecraftInputStream {
    public:
        MinecraftInputStream(unsigned char* buffer);
        int read_varint();
        std::string read_string();
    private:
        unsigned char* buffer;
        int pos;
    };
}
