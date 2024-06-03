#include <iostream>
#include <boost/asio.hpp>
#include <string>
#include <array>

using boost::asio::ip::tcp;

void communicate_with_server(const std::string& ip, const std::string& port) {
    try {
        // Create an io_context object to manage asynchronous operations
        boost::asio::io_context io_context;

        // Create a resolver to resolve the hostname to an IP address
        tcp::resolver resolver(io_context);

        // Resolve the hostname and port to get the endpoints
        auto endpoints = resolver.resolve(ip, port);

        // Create a socket and connect to the server
        tcp::socket socket(io_context);
        boost::asio::connect(socket, endpoints);

        // Send data to the server
        std::string message = "Hello, TCP Server!";
        boost::asio::write(socket, boost::asio::buffer(message));

        // Receive a response from the server
        std::array<char, 128> buffer;
        boost::system::error_code ec;
        unsigned int bytes_transferred = socket.read_some(boost::asio::buffer(buffer), ec);

        if (ec && ec != boost::asio::error::eof) {
            throw boost::system::system_error(ec);
        }

        // Display the response received
        std::cout << "Received response from server: "
                  << std::string(buffer.data(), bytes_transferred) << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        std::exit(1); // Exit with a non-zero exit code to indicate an error
    }
}

int main() {
    std::string ip, port;

    std::cout << "Enter IP address: ";
    std::cin >> ip;

    std::cout << "Enter Port: ";
    std::cin >> port;

    communicate_with_server(ip, port);

    return 0;
}
