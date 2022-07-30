#include "minecraft_write_stream.hpp"

namespace conduit {
    int calculate_varint_length(int value) {
        std::vector<mc_byte> buf;
        minecraft_write_stream write_stream {&buf};
        write_stream.write_varint(value);
        return buf.size();
    }
}