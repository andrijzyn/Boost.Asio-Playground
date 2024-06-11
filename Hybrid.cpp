#include "include/includes.h"

// Hosting function (pending connection)
void Host(boost::asio::io_service& io_service, const string& address, unsigned short port) {

    tcp::acceptor acceptor(io_service, tcp::endpoint(tcp::v4(), port));

    while (true) {
        shared_ptr<tcp::socket> socket = make_shared<tcp::socket>(io_service);
        acceptor.accept(*socket);

        // Get client endpoint
        tcp::endpoint clientEndpoint = socket->remote_endpoint();

        // Processing a connected client (e.g. in a separate thread)
        thread([socket, clientEndpoint]() {
            try {
                cout << "Client connected: " << clientEndpoint << endl;
                while (true) {
                    // Response | Echo_Server
                    boost::asio::streambuf receive_buffer;
                    boost::system::error_code ec;
                    boost::asio::read_until(*socket, receive_buffer, '\n', ec);
                    if (ec == boost::asio::error::eof) {
                        cout << "Client disconnected." << endl;
                        break;
                    } else if (ec) {
                        cerr << "Read error: " << ec.message() << endl;
                        break;
                    }

                    string message(boost::asio::buffer_cast<const char*>(receive_buffer.data()));
                    boost::asio::write(*socket, boost::asio::buffer(message));
                }
            } catch (const exception& e) {
                cerr << "# Exception in client handling: " << e.what() << endl;
            }
        }).detach();
    }
}

// Function for connecting to the remote host
shared_ptr<tcp::socket> Connect(boost::asio::io_service& io_service, const string& serverAddress, unsigned short port) {
    shared_ptr<tcp::socket> socket = make_shared<tcp::socket>(io_service);

    tcp::endpoint endpoint(boost::asio::ip::address::from_string(serverAddress), port);
    socket->connect(endpoint);

    return socket;
}

int main() {
    try {
        boost::asio::io_service io_service;

        // Output the IP address of the current host
        std::cout << "Your IP address: " << getIP() << std::endl;

        // Getting the "operating mode" from the user
        auto mode = get("Choose mode:\n1. Hosting\n2. Connecting\n: ");

        if (mode && *mode == "1") {
            // Hosting
            string address = getIP(); // IP address of the current host
            unsigned short port = 8080; // Port for listening
            thread t([&io_service, address, port]() {
                Host(io_service, address, port);
            });
            io_service.run();
            t.join(); // Wait for the hosting thread to finish
        } else if (mode && *mode == "2") {
            // Connecting
            auto serverAddress = get("Enter server IP: ");

            unsigned short port = 8080; // Port for connection
            std::shared_ptr<tcp::socket> socket = Connect(io_service, *serverAddress, port);

            // Sending a message to the server
            auto message = get("Enter message: ");
            boost::asio::write(*socket, boost::asio::buffer(*message));

            // Receiving a response from the server
            boost::asio::streambuf receive_buffer;
            boost::asio::read_until(*socket, receive_buffer, '\n');
            std::string response(boost::asio::buffer_cast<const char*>(receive_buffer.data()));
            std::cout << "Server response: " << response;

            io_service.run();
        } else {
            std::cerr << "Invalid mode selection." << std::endl;
            return 1;
        }

    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}