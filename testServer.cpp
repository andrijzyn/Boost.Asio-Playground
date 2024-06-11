#include "includes.h"

int main() {
    try {
        boost::asio::io_service io_service;
        tcp::acceptor acceptor(io_service, tcp::endpoint(tcp::v4(), 8080)); // Принимаем соединения на порту 8080

        cout << "Server started. Listening for incoming connections...\n";

        while (true) {
            tcp::socket socket(io_service);
            acceptor.accept(socket); // Принимаем входящее соединение

            cout << "New connection established.\n";
        }
    } catch (const exception& e) {
        cerr << "Exception: " << e.what() << endl;
    }

    return 0;
}
