#include "includes.h"

class P2PNode : public enable_shared_from_this<P2PNode> {
public:
    P2PNode(io_context& io_context, short port)
        : acceptor_(io_context, tcp::endpoint(tcp::v4(), port)),
          socket_(io_context), 
          timer_(io_context) {
        acceptor_.set_option(boost::asio::socket_base::reuse_address(true));
        do_accept();
    }

    void CONNECT(const string& address, short port) {
    auto self(shared_from_this());
    tcp::endpoint endpoint;
    try { endpoint = tcp::endpoint(ip::address::from_string(address), port);
    } catch (const boost::system::system_error& e) {
        cerr << "Invalid IP address: " << address << endl;
        auto address = Get("Enter peer ip to connect: ");
    }

    auto socket = make_shared<tcp::socket>(acceptor_.get_executor());

    timer_.expires_after(boost::asio::chrono::seconds(5));
    timer_.async_wait([this, self, socket](boost::system::error_code ec) {
        if (!ec) {
            socket->close();
            cerr << "Connection timed out." << endl;
        }
    });

    socket->async_connect(endpoint, [this, self, socket, address, port](boost::system::error_code ec) {
        if (ec) {
            cerr << "Failed to connect: " << ec.message() << endl;
            return;
        }

        timer_.cancel();
        cout << "Connected to " << address << ":" << port << endl;
        start_read(socket);
    });
}

private:
    void do_accept() {
        acceptor_.async_accept(socket_, [this](boost::system::error_code ec) {
            if (!ec) {
                timer_.cancel();
                cout << "Client connected: " << socket_.remote_endpoint() << endl;
                start_read(make_shared<tcp::socket>(move(socket_)));
            } else {
                cerr << "Error accepting connection: " << ec.message() << endl;
            }
            do_accept();
        });
    }

    void start_read(shared_ptr<tcp::socket> socket) {
        auto self(shared_from_this());
        async_read_until(*socket, boost::asio::dynamic_buffer(data_), "\n",
            [this, self, socket](boost::system::error_code ec, size_t length) {
                if (!ec) {
                    string message(data_.substr(0, length));
                    data_.erase(0, length);
                    cout << "Received: " << message << endl;

                    // Echo message back to peer
                    start_write(socket, message);
                } else {
                    cerr << "Error reading from socket: " << ec.message() << endl;
                    socket->close();
                }
            });
    }

    void start_write(shared_ptr<tcp::socket> socket, const string& message) {
        auto self(shared_from_this());
        async_write(*socket, boost::asio::buffer(message + "\n"),
            [this, self, socket](boost::system::error_code ec, size_t length) {
                if (!ec) {
                    start_read(socket); // Continue reading from the peer
                } else {
                    cerr << "Error writing to socket: " << ec.message() << endl;
                    socket->close();
                }
            });
    }

    tcp::acceptor acceptor_;
    tcp::socket socket_;
    boost::asio::steady_timer timer_;
    string data_;
};

int main() {
    try {
        io_context io_context;
        auto node = make_shared<P2PNode>(io_context, 8080);
        thread t([&io_context]() { io_context.run(); });

        auto address_ptr = Get("Enter peer ip to connect: ");
        if (!address_ptr) {
            cerr << "Error getting input" << endl;
            return 1;
        }
        string address = *address_ptr;

        node->CONNECT(address, 8080);

        t.join();
    } catch (exception& e) { cerr << "Exception: " << e.what() << endl; }

    return 0;
}