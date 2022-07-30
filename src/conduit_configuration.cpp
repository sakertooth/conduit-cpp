#include "conduit_configuration.hpp"

namespace conduit {
    
    auto conduit_configuration::s_timeout = std::chrono::milliseconds(0);
    auto conduit_configuration::s_scan_size = 0;
    auto conduit_configuration::s_buffer_size = 0;
    auto conduit_configuration::s_ipv6 = false;
    auto conduit_configuration::s_output_dir = std::string{""};
    auto conduit_configuration::s_verbose = false;

    std::chrono::milliseconds& conduit_configuration::timeout() {
        return s_timeout;
    }

    int conduit_configuration::scan_size() {
        return s_scan_size;
    }

    int conduit_configuration::buffer_size() {
        return s_buffer_size;
    }

    bool conduit_configuration::ipv6() {
        return s_ipv6;
    }

    std::string& conduit_configuration::output_dir() {
        return s_output_dir;
    }

    bool conduit_configuration::verbose() {
        return s_verbose;
    }

    void conduit_configuration::set_timeout(const std::chrono::milliseconds& timeout) {
        s_timeout = timeout;
    }
    
    void conduit_configuration::set_scan_size(int scan_size) {
        s_scan_size = scan_size;
    }
    
    void conduit_configuration::set_buffer_size(int buffer_size) {
        s_buffer_size = buffer_size;
    }
    
    void conduit_configuration::set_ipv6(bool ipv6) {
        s_ipv6 = ipv6;
    }
    
    void conduit_configuration::set_output_dir(const std::string& output_dir) {
        s_output_dir = output_dir;
    }

    void conduit_configuration::set_verbose(bool verbose) {
        s_verbose = verbose;
    }

}