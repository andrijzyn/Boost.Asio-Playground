#include <iostream>
#include <boost/asio.hpp>

using namespace boost::asio;
using ip::tcp;

int main() {
    try {
        boost::asio::io_service io_service;

        // Создание объекта resolver для разрешения имени хоста в IP-адрес
        tcp::resolver resolver(io_service);

        // Разрешение имени хоста и порта
        tcp::resolver::results_type endpoints = resolver.resolve("localhost", "4444");

        // Создание сокета и подключение к серверу
        tcp::socket socket(io_service);
        boost::asio::connect(socket, endpoints);

        // Отправка данных на сервер
        std::string message = "Hello, TCP Server!";
        boost::asio::write(socket, boost::asio::buffer(message));

        // Получение ответа от сервера
        std::array<char, 128> buffer;
        size_t bytes_transferred = socket.read_some(boost::asio::buffer(buffer));
        std::cout << "Received response from server: " << std::string(buffer.data(), bytes_transferred) << std::endl;

    } catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
    return 0;
}
