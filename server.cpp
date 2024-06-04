#include <iostream>
#include <string>
#include <stdexcept>
#include <boost/asio.hpp>

#if defined(_WIN32) || defined(_WIN64)
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")
#else
    #include <array>
    #include <memory>
    #include <cstdio>
#endif

using namespace std;
using namespace boost::asio;
using namespace boost::asio::ip;

string get_ip_address() {
#if defined(_WIN32) || defined(_WIN64)
    WSADATA wsaData;
    char hostname[256];
    char ip_address[256];
    struct addrinfo *result = nullptr, *ptr = nullptr, hints;
    struct sockaddr_in *sockaddr_ipv4;
    void *addr_ptr;

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        throw runtime_error("WSAStartup failed");
    }

    if (gethostname(hostname, sizeof(hostname)) == SOCKET_ERROR) {
        WSACleanup();
        throw runtime_error("gethostname failed");
    }

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    if (getaddrinfo(hostname, NULL, &hints, &result) != 0) {
        WSACleanup();
        throw runtime_error("getaddrinfo failed");
    }

    for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {
        sockaddr_ipv4 = (struct sockaddr_in *)ptr->ai_addr;
        addr_ptr = &(sockaddr_ipv4->sin_addr);
        inet_ntop(AF_INET, addr_ptr, ip_address, sizeof(ip_address));
        break;
    }

    freeaddrinfo(result);
    WSACleanup();
    return string(ip_address);
#else
    const char* command = "hostname -I | awk '{print $1}'";
    array<char, 128> buffer;
    string result;

    shared_ptr<FILE> pipe(popen(command, "r"), pclose);
    if (!pipe) {
        throw runtime_error("popen() failed!");
    }

    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }

    result.erase(result.find_last_not_of(" \n\r\t") + 1);

    return result;
#endif
}

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

        cout << "// Hello there, this is TCP Server client writed by Zyniuk Andrij\n Wait for client connection...\n";

        // Display local IP address
        try {
            string ip_address = get_ip_address();
            cout << "This is your IP address: " << ip_address << endl;
        } catch (const exception& e) {
            cerr << "Exception: " << e.what() << endl;
            return 1;
        }

        cout << "Enter port for server: ";
        cin >> PORT;

        TCPServer server(io_service, PORT);
        io_service.run();
    } catch (exception& e) {
        cerr << "Exception: " << e.what() << endl;
    }
    return 0;
}
