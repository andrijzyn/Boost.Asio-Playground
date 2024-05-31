#include <iostream>
#include <boost/asio.hpp>

using namespace boost::asio;
using ip::tcp;

short PORT = 4444;

class TCPServer {
public:
    TCPServer(io_service& io_service, short port) : acceptor_(io_service, tcp::endpoint(tcp::v4(), port)), socket_(io_service) {
        do_accept();
    }

private:
    void do_accept() {
        acceptor_.async_accept(socket_, [this](boost::system::error_code ec) {
            if (!ec) {
                std::cout << "Client connected: " << socket_.remote_endpoint() << std::endl;
                do_read();
            } else {
                std::cerr << "Error accepting connection: " << ec.message() << std::endl;
            }
        });
    }

    void do_read() {
        socket_.async_read_some(buffer(data_, max_length), [this](boost::system::error_code ec, std::size_t length) {
            if (!ec) {
                do_write(length);
            } else {
                std::cerr << "Error reading from socket: " << ec.message() << std::endl;
            }
        });
    }

    void do_write(std::size_t length) {
        async_write(socket_, buffer(data_, length), [this](boost::system::error_code ec, std::size_t /*length*/) {
            if (!ec) {
                do_read();
            } else {
                std::cerr << "Error writing to socket: " << ec.message() << std::endl;
            }
        });
    }

    tcp::acceptor acceptor_;
    tcp::socket socket_;
    enum { max_length = 1024 };
    char data_[max_length];
};

int main() {
    try {
        boost::asio::io_service io_service;
        TCPServer server(io_service, PORT);
        io_service.run();
    } catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
    return 0;
}
