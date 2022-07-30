#pragma once
#include <chrono>
#include <string>

namespace conduit {
    class conduit_configuration {
    public:
        static std::chrono::milliseconds& timeout();
        static int scan_size();
        static int buffer_size();
        static bool ipv6();
        static std::string& output_dir();
        static bool verbose();

        static void set_timeout(const std::chrono::milliseconds& timeout);
        static void set_scan_size(int scan_size);
        static void set_buffer_size(int buffer_size);
        static void set_ipv6(bool ipv6);
        static void set_output_dir(const std::string& output_dir);
        static void set_verbose(bool verbose); 
    private:
        static std::chrono::milliseconds s_timeout;
        static int s_scan_size;
        static int s_buffer_size;
        static bool s_ipv6;
        static std::string s_output_dir;
        static bool s_verbose;
    };
}