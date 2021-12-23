#include <iostream>
#include <argparse/argparse.hpp>
#include <boost/asio.hpp>
#include <chrono>
#include "scanworker.hpp"
#include "helpers.hpp"

int main(int argc, char **argv) {
    argparse::ArgumentParser parser {"Conduit"};

    parser.add_argument("target")
        .help("the target range");

    parser.add_argument("-p", "--ports")
        .default_value<std::string>("25565")
        .help("the port range");

    try {
        parser.parse_args(argc, argv);
    }
    catch (const std::runtime_error& err) {
        std::cerr << err.what() << '\n';
        std::cerr << parser;
        std::exit(1);
    }

    const auto& ip_target_range = parser.get("target");
    const auto& port_range = parser.get("--ports");

    boost::asio::io_context main_io_context;

    conduit::MinecraftScanWorker worker{main_io_context, boost::asio::ip::tcp::endpoint(boost::asio::ip::address::from_string("147.135.8.205"), 25565)};
    worker.start();

    const auto& start_time = std::chrono::steady_clock::now();
    const unsigned int num_of_cores = std::thread::hardware_concurrency();  
    std::vector<std::thread> threads(num_of_cores);

    for (int i = 0; i < 1; i++) {
        threads.push_back(std::thread {[&main_io_context](){ main_io_context.run(); }});
    }

    for (auto& thread : threads) {
        if (thread.joinable()) {
            thread.join();
        }
    }

    const auto& end_time = std::chrono::steady_clock::now();
    std::cout << "Conduit finished in " << std::chrono::duration<float, std::chrono::seconds::period>(end_time - start_time).count() << " seconds\n";
}
