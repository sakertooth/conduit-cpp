#pragma once
#include <string>
#include <fstream>
#include <any>
#include <vector>
#include <iostream>

namespace conduit {
    class conduit_logger {
    public:
        static void open(const std::string& filename) {
            s_output_stream = std::ofstream {filename};
            if (!s_output_stream) {
                throw std::runtime_error {"Failed to open output file " + filename};
            }
        }

        static void close() {
            s_output_stream.close();
        }

        template <typename T>
        friend conduit_logger& operator<<(conduit_logger& stream, const T& obj) {
            std::cout << obj;
            if (s_output_stream.is_open()) {
                conduit_logger::s_output_stream << obj;
            }
            return stream;    
        }

        static conduit_logger& instance() {
            return s_conduit_logger;
        }
    private:
        static std::ofstream s_output_stream;
        static conduit_logger s_conduit_logger;
    };
}