#ifndef FILE2_H // Stackoverflow helped :)
#define FILE2_H
#include <iostream>
#include <string>
#include <stdexcept>
#include <boost/asio.hpp>
#include <memory>

#if defined(_WIN32) || defined(_WIN64)
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")
#else
    #include <array>
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

#endif // FILE2_H