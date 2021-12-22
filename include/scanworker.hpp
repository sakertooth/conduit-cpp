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

        void on_connect(const boost::system::error_code& ec, const boost::asio::ip::tcp::endpoint& endpoint);
        void on_write(const boost::system::error_code& ec, std::size_t bytes_transferred);
        void on_read(const boost::system::error_code& ec, std::size_t bytes_transferred);
        
        std::vector<unsigned char> build_request();

        std::vector<unsigned char> read_buffer;
        boost::asio::ip::tcp::endpoint endpoint;
        boost::asio::ip::tcp::socket socket;
        WorkerState state;
    };

    class MinecraftScan {

    };
}