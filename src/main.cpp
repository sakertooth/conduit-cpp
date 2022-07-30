#include "argparse/argparse.hpp"
#include "net_parser.hpp"
#include "minecraft_ping_task.hpp"
#include "asio.hpp"
#include "conduit_configuration.hpp"
#include "conduit_logger.hpp"
#include <chrono>
#include <ctime>
#include <cmath>

int main(int argc, char **argv) {
    argparse::ArgumentParser parser {"Conduit 2.0"};

    parser.add_argument("target")
        .help("The target range");

    parser.add_argument("-p", "--ports")
        .default_value<std::string>("25565")
        .help("The port range");

    parser.add_argument("-t", "--timeout")
        .help("Timeout in milliseconds for connecting to and reading from servers")
        .default_value<int>(1000)
        .scan<'i', int>();

    parser.add_argument("-s", "--scan-size")
        .help("The maximum # of scans occuring at a time")
        .default_value<int>(256)
        .scan<'i', int>();

    parser.add_argument("-b", "--buffer-size")
        .help("The size of buffer used to read from TCP streams")
        .default_value<int>(1024)
        .scan<'i', int>();

    parser.add_argument("-i", "--ipv6")
        .help("IPv6 mode")
        .default_value(false)
        .implicit_value(true);

    parser.add_argument("-o", "--output")
        .default_value<std::string>("")
        .help("Output to file");

    parser.add_argument("-v", "--verbose")
        .help("Verbose mode")
        .default_value(false)
        .implicit_value(true);
        
    try {
        parser.parse_args(argc, argv);
    }
    catch (const std::runtime_error& err) {
        std::cerr << err.what() << '\n';
        std::cerr << parser;
        std::exit(EXIT_FAILURE);
    }

    auto timeout = std::chrono::milliseconds {parser.get<int>("--timeout")};
    int scan_size = parser.get<int>("--scan-size");
    int buffer_size = parser.get<int>("--buffer-size");
    bool isIPV6 = parser.get<bool>("--ipv6");
    bool verbose = parser.get<bool>("--verbose");
    auto output_dir = parser.get<std::string>("--output");

    conduit::conduit_configuration::set_timeout(timeout);
    conduit::conduit_configuration::set_scan_size(scan_size);
    conduit::conduit_configuration::set_buffer_size(buffer_size);
    conduit::conduit_configuration::set_ipv6(isIPV6);
    conduit::conduit_configuration::set_verbose(verbose);
    conduit::conduit_configuration::set_output_dir(output_dir);

    if (!output_dir.empty()) {
        conduit::conduit_logger::open(output_dir);
    }
    
    std::variant<asio::ip::network_v4, asio::ip::network_v6> target_range;
    std::vector<unsigned short> port_range;
    try {
        target_range = conduit::parse_targetrange(parser.get("target"), isIPV6);
        port_range = conduit::parse_portrange(parser.get("--ports"));
    }
    catch (const std::exception& ex) {
        std::cerr << "Failed to parse target/port range: " << ex.what() << '\n';
        std::exit(EXIT_FAILURE);
    }

    int num_servers_found = 0;
    auto time_start = std::chrono::steady_clock::now();

    std::visit([&](auto&& arg) {
        auto hosts = arg.hosts();
        auto num_hosts = std::distance(hosts.begin(), hosts.end());
        scan_size = num_hosts < scan_size ? num_hosts : scan_size;
        int num_batches = static_cast<int>(std::ceil(num_hosts / static_cast<float>(scan_size)));

        auto time = std::time(nullptr);
        conduit::conduit_logger::instance() << "Starting Conduit on " << std::ctime(&time);

        if (verbose) {
            conduit::conduit_logger::instance() << "Scan size: " << scan_size << " host(s) per batch [" << num_batches << " batch(es)]\n";
        }

        auto process_tasks = [&](asio::io_context& io, std::vector<conduit::minecraft_ping_task>& tasks) {
            for (auto& task : tasks) {
                task.start();
            }

            io.run();
            io.restart();
            num_servers_found += std::count_if(tasks.begin(), tasks.end(), [](auto& task) { return task.succeeded(); });
            tasks.clear();
        };

        asio::io_context io;
        std::vector<conduit::minecraft_ping_task> tasks;
        for (auto& host : hosts) {
            for (auto port : port_range) {
                tasks.emplace_back(io, host, port);
                if (tasks.size() == static_cast<std::size_t>(scan_size)) {
                    process_tasks(io, tasks);
                }
            }
        }

        if (tasks.size() > 0) {
            process_tasks(io, tasks);
        }

    }, target_range);

    auto time_end = std::chrono::steady_clock::now();
    auto duration_ms = std::chrono::duration<double>(time_end - time_start);
    
    conduit::conduit_logger::instance() << "Conduit finished: " 
        << num_servers_found << " server(s) found in " 
        << duration_ms.count() << " seconds.\n";

    conduit::conduit_logger::close();
}
