#include <iostream>
#include <string>
#include <stdexcept>
#include <boost/asio.hpp>
#include <memory>
#include <limits>
#include "include/get_ip.h"

using namespace std;
using namespace boost::asio;
using namespace boost::asio::ip;

#if defined(_WIN32) || defined(_WIN64)
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")
#else
    #include <array>
    #include <cstdio>
#endif

class TCPServer : public std::enable_shared_from_this<TCPServer> {
public:
    TCPServer(io_service& io_service    , short port)
        : acceptor_(io_service, tcp::endpoint(tcp::v4(), port)),
          socket_(io_service) {
        do_accept();
    }

private:
    void do_accept() {
        acceptor_.async_accept(socket_, [this](boost::system::error_code ec) {
            if (!ec) {
                cout << "\nClient connected: " << socket_.remote_endpoint() << endl;
                start_read();
            } else {
                cerr << "Error accepting connection: " << ec.message() << endl;
                do_accept(); // Continue accepting new connections
            }
        });
    }

    void start_read() {
        auto self(shared_from_this());
        async_read_until(socket_, boost::asio::dynamic_buffer(data_), "\n",
            [this, self](boost::system::error_code ec, size_t length) {
                if (!ec) {
                    string message(data_.substr(0, length));
                    data_.erase(0, length);
                    cout << "Received: " << message << endl;

                    // Process the received data
                    start_write(message);
                } else if (ec == boost::asio::error::eof) {
                    cout << "Client disconnected.\n" << endl;
                    socket_.close();
                    do_accept(); // Continue accepting new connections
                } else {
                    cerr << "Error reading from socket: " << ec.message() << endl;
                    socket_.close();
                    do_accept(); // Continue accepting new connections
                }
            });
    }

    void start_write(const string& message) {
        auto self(shared_from_this());
        async_write(socket_, boost::asio::buffer(message + "\n"),
            [this, self](boost::system::error_code ec, size_t length) {
                if (!ec) {
                    cout << "Message sent to client.\n" << endl;
                    start_read(); // Continue reading from the client
                } else {
                    cerr << "Error writing to socket: " << ec.message() << endl;
                    socket_.close();
                    do_accept(); // Continue accepting new connections
                }
            });
    }

    tcp::acceptor acceptor_;
    tcp::socket socket_;
    std::string data_;
};

int main() {
    try 
    {
        io_service io_service;

        string ip_address = get_ip_address();
        cout << "Server started. You'r local IP address: " << ip_address << endl;
        auto server = std::make_shared<TCPServer>(io_service, 8080);
        io_service.run();

    } catch (exception& e) {cerr << "Exception: " << e.what() << endl;}
    return 0;
}
