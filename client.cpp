//try {
//    } catch (exception& e) { cerr << "Exception " << e.what() << endl; }

#include "include/includes.h"

std::shared_ptr<tcp::socket> Core(io_service& io_service) { 
    try {
        auto socket = std::make_shared<tcp::socket>(io_service);

        auto CLI_ADDRESS = Get("Enter server IP: ");
        if (!CLI_ADDRESS) {
            cerr << "Failed to get server IP." << endl;
            return nullptr;
        }

        tcp::endpoint endpoint(address::from_string(*CLI_ADDRESS), 8080);
        socket->connect(endpoint); // Connect to the server
        cout << "You have been connected to the server.\n\n";

        return socket;
    } catch (const std::exception& e) {
        cerr << "Exception: " << e.what() << endl;
        return nullptr;
    }
}

int main() {
    try {
        io_service io_service;
        auto socket = Core(io_service);

        if (!socket) {
            cerr << "Failed to initialize socket." << endl;
            return 1;
        }

        while (true) {
            // Send message to server
            auto MESSAGE = Get("Enter message: ");
            if (!MESSAGE) {
                cerr << "Failed to get message." << endl;
                continue;
            }
            MESSAGE->append("\n");
            write(*socket, boost::asio::buffer(*MESSAGE));

            // Receive response from server
            boost::asio::streambuf receive_buffer;
            read_until(*socket, receive_buffer, '\n');
            string response = buffer_cast<const char*>(receive_buffer.data());
            cout << "Server: " << response << endl;
        }
    } catch (const exception& e) { 
        cerr << "Exception: " << e.what() << endl; 
        return 1;
    }

    return 0;
}