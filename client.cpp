    #include <iostream>
    #include <boost/asio.hpp>

    using namespace std;
    using namespace boost::asio;
    using namespace boost::asio::ip;

    int main() {
        try {
            io_service io_service;
            tcp::socket socket(io_service);
            string CLIENT_ADDRESS, message;
            
            cout << "Please, Enter the client IP address :)" << endl;
            getline(cin, CLIENT_ADDRESS);

            tcp::endpoint endpoint(address::from_string(CLIENT_ADDRESS), 8080); // address::from_string("127.0.0.1")

            // Connect to the server
            socket.connect(endpoint);

            cout << "You have been connected to the server.\n\n";

            // Enter a loop to send messages to the server
            while (true) {
                cout << "You: ";
                // string message;
                getline(cin, message);

                // Send message to server
                write(socket, boost::asio::buffer(message + "\n"));

                // Receive response from server
                boost::asio::streambuf receive_buffer;
                read_until(socket, receive_buffer, '\n');
                string response = buffer_cast<const char*>(receive_buffer.data());

                cout << "Server: " << response << endl;
            }
        } catch (exception& e) {
            cerr << "Exception: " << e.what() << endl;
        }

        return 0;
    }
