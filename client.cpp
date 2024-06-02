#include <iostream>
#include <boost/asio.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <string>
#include <array>

using boost::asio::ip::tcp;

int main() {
    try {
        // Create an io_service object to manage asynchronous operations
        boost::asio::io_service io_service;

        // Create a resolver to resolve the hostname to an IP address
        tcp::resolver resolver(io_service);

        // Resolve the hostname and port to get the endpoints
        tcp::resolver::results_type endpoints = resolver.resolve("localhost", "4444");

        // Create a socket and connect to the server
        tcp::socket socket(io_service);
        connect(socket, endpoints);

        // Send data to the server
        std::string message = "Hello, TCP Server!";
        write(socket, boost::asio::buffer(message));

        // Receive a response from the server
        std::array<char, 128> buffer;
        boost::system::error_code ec;
        size_t bytes_transferred = socket.read_some(boost::asio::buffer(buffer), ec);

        if (ec) {
            throw boost::system::system_error(ec);
        }

        // Display the response received
        std::cout << "Received response from server: " << std::string(buffer.data(), bytes_transferred);

    } catch (const std::exception& e) {
        // Handle any exceptions and display an error message
        std::cerr << "Exception " << e.what() << std::endl;
        return 1; // Return a non-zero exit code to indicate an error
    }

    return 0;
}