#include "scanworker.hpp"
#include "varint.hpp"
#include <boost/bind/bind.hpp>
#include <iostream>

conduit::MinecraftScanWorker::MinecraftScanWorker(boost::asio::io_context& io_context, const boost::asio::ip::tcp::endpoint& endpoint) : 
    socket(io_context), endpoint(endpoint), state(WorkerState::none) {}

void conduit::MinecraftScanWorker::start() {
    socket.async_connect(endpoint, boost::bind(&MinecraftScanWorker::on_connect, this, boost::asio::placeholders::error, endpoint));
    state = WorkerState::connecting;
}

void conduit::MinecraftScanWorker::on_connect(const boost::system::error_code& ec, const boost::asio::ip::tcp::endpoint& endpoint) {
    if (ec) {
        socket.close();
        state = WorkerState::failed;
        return;
    }
    
    auto request = build_request();
    boost::asio::async_write(socket, boost::asio::buffer(request), boost::asio::transfer_all(), 
        boost::bind(&MinecraftScanWorker::on_write, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
    state = WorkerState::writing;
}

void conduit::MinecraftScanWorker::on_write(const boost::system::error_code& ec, std::size_t bytes_transferred) {
    if (ec) {
        socket.close();
        state = WorkerState::failed;
        return;
    }

    unsigned char packet_length[5];
    std::memset(packet_length, 0, sizeof(packet_length));

    boost::asio::async_read(socket, boost::asio::buffer(packet_length, sizeof(packet_length)), boost::asio::transfer_exactly(sizeof(packet_length)),
        [&](const boost::system::error_code& ec, std::size_t bytes_transferred) {

            for (auto& c : packet_length) {
                std::cout << (int)c << " ";
            }
            std::cout << '\n';
            int val = conduit::decode_varint(packet_length);
            std::cout << val << '\n';
        });
}

void conduit::MinecraftScanWorker::on_read(const boost::system::error_code& ec, std::size_t bytes_transferred) {
    if (ec) {
        socket.close();
        state = WorkerState::failed;
        return;
    }

}

std::vector<unsigned char> conduit::MinecraftScanWorker::build_request() {
    std::vector<unsigned char> request;

    request.push_back(0x00);
    request.push_back(0x47);

    std::string address = endpoint.address().to_string();
    auto address_length_varint = conduit::encode_varint(address.length());

    request.insert(request.end(), address_length_varint.begin(), address_length_varint.end());
    request.insert(request.end(), address.begin(), address.end());

    unsigned short port = endpoint.port();
    request.push_back(port << 8);
    request.push_back(port >> 8);

    request.push_back(0x01);

    auto handshake_length_varint = conduit::encode_varint(request.size());
    request.insert(request.begin(), handshake_length_varint.begin(), handshake_length_varint.end());
    
    request.push_back(0x01);
    request.push_back(0x00);

    return request;
}

conduit::MinecraftScanWorker::~MinecraftScanWorker() {
    socket.close();
}