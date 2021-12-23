#include "scanworker.hpp"
#include "helpers.hpp"
#include <boost/bind/bind.hpp>
#include <iostream>

conduit::MinecraftScanWorker::MinecraftScanWorker(boost::asio::io_context& io_context, const boost::asio::ip::tcp::endpoint& endpoint) : 
    socket(io_context), endpoint(endpoint), state(WorkerState::none) {}

void conduit::MinecraftScanWorker::start() {
    socket.async_connect(endpoint, boost::bind(&MinecraftScanWorker::handle_connect, this, boost::asio::placeholders::error, endpoint));
    state = WorkerState::connecting;
}

void conduit::MinecraftScanWorker::handle_connect(const boost::system::error_code& ec, const boost::asio::ip::tcp::endpoint& endpoint) {
    if (ec) {
        socket.close();
        state = WorkerState::failed;
        return;
    }

    auto request = build_request();
    boost::asio::async_write(socket, boost::asio::buffer(request), boost::asio::transfer_all(), 
        boost::bind(&MinecraftScanWorker::handle_write, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
    state = WorkerState::writing;
}

void conduit::MinecraftScanWorker::handle_write(const boost::system::error_code& ec, std::size_t bytes_transferred) {
    if (ec) {
        socket.close();
        state = WorkerState::failed;
        return;
    }

    auto read_buffer = std::make_shared<std::vector<std::byte>>(1024);
    boost::asio::async_read(socket, boost::asio::buffer(*read_buffer), boost::asio::transfer_at_least(5), 
        boost::bind(&MinecraftScanWorker::handle_read, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred, read_buffer));
    state = WorkerState::reading_packet_length;

}

void conduit::MinecraftScanWorker::handle_read(const boost::system::error_code& ec, std::size_t bytes_transferred, std::shared_ptr<std::vector<std::byte>> read_buffer) {
    if (ec) {
        socket.close();
        state = WorkerState::failed;
        return;
    }

    auto packet_length = std::make_shared<int>(conduit::read_varint(read_buffer->data()));
    read_buffer->resize(read_buffer->size() + *packet_length);

    boost::asio::async_read(socket, boost::asio::buffer(*read_buffer), boost::asio::transfer_exactly(*packet_length), [&](const boost::system::error_code& ec, std::size_t bytes_transferred) {
            std::cout << "woo: " << *packet_length << '\n';        
    });
}

std::vector<std::byte> conduit::MinecraftScanWorker::build_request() {
    std::vector<std::byte> request;

    request.push_back(static_cast<std::byte>(0x00));
    request.push_back(static_cast<std::byte>(0x47));

    auto address = conduit::write_string(endpoint.address().to_string());
    request.insert(request.end(), address.begin(), address.end());

    request.push_back(static_cast<std::byte>(endpoint.port() << 8));
    request.push_back(static_cast<std::byte>(endpoint.port() >> 8));

    request.push_back(static_cast<std::byte>(0x01));

    auto handshake_length = conduit::write_varint(request.size());
    request.insert(request.begin(), handshake_length.begin(), handshake_length.end());

    request.push_back(static_cast<std::byte>(0x01));
    request.push_back(static_cast<std::byte>(0x00));
    return request;
}

conduit::MinecraftScanWorker::~MinecraftScanWorker() {
    socket.close();
}