#include <iostream>
#include <string>
#include <boost/asio.hpp>

using namespace std;
using namespace boost::asio;
using namespace boost::asio::ip;

class TCPServer {
public:
    TCPServer(io_service& io_service, short port)
        : acceptor_(io_service, tcp::endpoint(tcp::v4(), port)),
          socket_(io_service) {
        do_accept();
    }

private:
    void do_accept() {
        acceptor_.async_accept(socket_, [this](boost::system::error_code ec) {
            if (!ec) {
                cout << "Client connected: " << socket_.remote_endpoint() << endl;
                do_write();
            } else {
                cerr << "Error accepting connection: " << ec.message() << endl;
            }
        });
    }

    void do_write() {
        string message = "Hello, client!";
        async_write(socket_, buffer(message),
            [this](boost::system::error_code ec, size_t /*length*/) {
                if (!ec) {
                    socket_.close();
                    cout << "Message sent. Client disconnected.\n" << endl;
                    do_accept();
                } else {
                    cerr << "Error writing to socket: " << ec.message() << endl;
                }
            });
    }

    tcp::acceptor acceptor_;
    tcp::socket socket_;
};

int main() {
    try {
        int PORT;
        io_service io_service;

        cout << "Enter Server Port: ";
        cin >> PORT;

        TCPServer server(io_service, PORT);
        io_service.run();
    } catch (exception& e) {
        cerr << "Exception: " << e.what() << endl;
    }
    return 0;
}
