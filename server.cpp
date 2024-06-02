#include <iostream>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

const short PORT = 4444;

class TCPServer {
public:
    TCPServer(boost::asio::io_service& io_service, short port)
        : acceptor_(io_service, tcp::endpoint(tcp::v4(), port)),
          socket_(io_service) {
        do_accept();
    }

private:
    void do_accept() {
        acceptor_.async_accept(socket_, [this](boost::system::error_code ec) {
            if (!ec) {
                std::cout << "Client has been connected: " << socket_.remote_endpoint() << std::endl;
                do_write();
            } else {
                std::cerr << "Error accepting connection: " << ec.message() << std::endl;
            }
        });
    }

    void do_write() {
        std::string message = "Hello, client!\n";
        boost::asio::async_write(socket_, boost::asio::buffer(message),
            [this](boost::system::error_code ec, std::size_t /*length*/) {
                if (!ec) {
                    socket_.close();
                    std::cout << "Client has been disconnected. The response was received.\n" << std::endl;
                    do_accept();
                } else {
                    std::cerr << "Error writing to socket: " << ec.message() << std::endl;
                }
            });
    }

    tcp::acceptor acceptor_;
    tcp::socket socket_;
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