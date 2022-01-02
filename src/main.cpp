#include "argparse/argparse.hpp"
#include "conduit/net_parser.hpp"
#include "conduit/mc_ping.hpp"
#include <boost/asio.hpp>

namespace asio = boost::asio;

int main(int argc, char **argv) {
    argparse::ArgumentParser parser {"Conduit"};

    parser.add_argument("target")
        .help("the target range");

    parser.add_argument("-p", "--ports")
        .default_value<std::string>("25565")
        .help("the port range");

    parser.add_argument("-t", "--timeout")
        .help("timeout in milliseconds for connecting, writing, and reading")
        .default_value<int>(1000)
        .scan<'i', int>();

    parser.add_argument("-s", "--size")
        .help("the maximum # of scans occuring at a time")
        .default_value<int>(256)
        .scan<'i', int>();
        
    try {
        parser.parse_args(argc, argv);

        auto target_range = conduit::parse_targetrange(parser.get("target"));
        auto port_range = conduit::parse_portrange(parser.get("--ports"));
        auto timeout = std::chrono::milliseconds(parser.get<int>("--timeout"));
        auto size = parser.get<int>("--size");
        asio::io_context io;

        const auto start_time = std::chrono::steady_clock::now();

        int num_of_tasks = 0;
        int servers_found = 0;
        for (auto&& ip : target_range.hosts()) {
            for (auto&& port : port_range) {
                if (num_of_tasks < size) {
                    asio::co_spawn(io, conduit::ping_mc_server_with_parse(std::move(ip), std::move(port), timeout), [&](auto eptr, bool success) {
                        if (success)
                            ++servers_found;
                    });
                    ++num_of_tasks;
                }
                else {
                    io.restart();
                    io.run();
                    num_of_tasks = 0;
                }
            }
        }

        io.restart();
        io.run();

        const auto end_time = std::chrono::steady_clock::now();
        std::cout << "Conduit found " << servers_found << " server" << (servers_found != 1 ? "s" : "") << " in " 
                    << std::chrono::duration<float, std::chrono::seconds::period>(end_time - start_time).count() << " seconds\n";
    }
    catch (const std::runtime_error& err) {
        std::cerr << err.what() << '\n';
        std::cerr << parser;
    }
}
