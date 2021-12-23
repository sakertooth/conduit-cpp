#include <boost/asio.hpp>
#include <ostream>
#include <istream>

namespace conduit {
    class MinecraftScanWorker {
    public:
        enum class WorkerState { none, connecting, failed, writing, reading_packet_length, reading_packet, complete };
        
        MinecraftScanWorker(boost::asio::io_context &io_context, const boost::asio::ip::tcp::endpoint &endpoint);
        ~MinecraftScanWorker();

        void start();

    private:

        void handle_connect(const boost::system::error_code& ec, const boost::asio::ip::tcp::endpoint& endpoint);
        void handle_write(const boost::system::error_code& ec, std::size_t bytes_transferred);
        void handle_read(const boost::system::error_code& ec, std::size_t bytes_transferred, std::shared_ptr<std::vector<std::byte>> read_buffer);
        
        std::vector<std::byte> build_request();

        boost::asio::ip::tcp::endpoint endpoint;
        boost::asio::ip::tcp::socket socket;
        WorkerState state;
    };

    class MinecraftScan {

    };
}