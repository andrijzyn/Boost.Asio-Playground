#include "include/includes.h"

// Hosting function (pending connection)
void Host(boost::asio::io_service& io_service, const string& address, unsigned short port) {
    tcp::acceptor acceptor(io_service, tcp::endpoint(boost::asio::ip::address::from_string(address), port));

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
            // Close the socket when the client disconnects
            socket->close();
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

// Function to send and receive messages
void Communicate(const std::shared_ptr<tcp::socket>& socket) {
    while (true) {
        auto message = get("Enter message: ");
        if (*message == "0") {
            break;
        }
        boost::asio::write(*socket, boost::asio::buffer(*message)); // Send message

        boost::asio::streambuf receive_buffer;
        boost::system::error_code error;
        boost::asio::read_until(*socket, receive_buffer, '\n', error); // Receive message

        if (error) {
            if (error == boost::asio::error::eof) {
                cout << "Connection closed by server." << endl;
            } else {
                cerr << "Error on receive: " << error.message() << endl;
            }
            break;
        }

        std::string response(boost::asio::buffer_cast<const char*>(receive_buffer.data()));
        std::cout << "Server response: " << response;
    }
}

int main() {
    unsigned short port = 8080;
    try {
        boost::asio::io_service io_service;

        // Output the IP address of the current host
        std::cout << "Your IP address: " << getIP() << std::endl;

        // Getting the "operating mode" from the user
        auto mode = get("Choose mode:  (1. Hosting), (2. Connecting), (Ctrl+C. Close): ");

        if (mode && *mode == "1") { // Hosting
            string address = getIP(); // IP address of the current host

            thread host_thread([&io_service, address, port]() {
                Host(io_service, address, port);
            });
            // Run the io_service to handle incoming connections
            io_service.run();
            host_thread.join(); // Wait for the host thread to finish (although it runs infinitely)
        } else if (mode && *mode == "2") { // Connecting
            auto serverAddress = get("Enter server IP: ");

            std::shared_ptr<tcp::socket> socket = Connect(io_service, *serverAddress, port);
            cout << "You have been connected to the server.\n\n";

            // Communicate with the server
            thread comm_thread([socket]() {
                Communicate(socket);
            });
            comm_thread.join(); // Wait for the communication thread to finish
            io_service.run(); // Run the io_service to handle asynchronous events
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
