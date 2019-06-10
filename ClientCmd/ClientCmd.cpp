// ClientCmd.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include "pch.h"
#include <iostream>
#include <boost/asio.hpp>
#include "../libs/Packet.h"

using boost::asio::ip::tcp;

void read_mes(tcp::socket & sock_)
{
    int got_bytes = 0;
    Packet command;
    boost::system::error_code ec;
#pragma pack(push,1)
    struct com {
        uint8_t cod;
        uint32_t length;
        uint16_t ident;
        uint16_t itemCount;
    } hdr;
#pragma pack(pop)
    char mas[9];
    
        if (got_bytes += boost::asio::read(sock_, boost::asio::buffer(&mas, sizeof(hdr)), boost::asio::transfer_at_least(1), ec)) {
            if (ec) {
                std::cout << "ERROR1: failed to read data: " << ec.message() << std::endl;
                return;
            }
            memcpy(&hdr, mas, 9);
            command.init(hdr.cod, hdr.length, hdr.ident, hdr.itemCount);
            if (hdr.cod != 131) {
            got_bytes += boost::asio::read(sock_, boost::asio::buffer(command.payload(), hdr.length - sizeof(hdr)), boost::asio::transfer_at_least(1), ec);
            if (ec) {
                std::cout << "ERROR2: failed to read data: " << ec.message() << std::endl;
                return;
            }
        }
        for (int i = 0; i < got_bytes; i++) {
            printf("%02X ", *(command.data() + i));
        }
        printf("\n");
        /*if (got_bytes == hdr.length)
        {
            process_request(std::move(command));
        }*/
    }
}

int main(int argc, char** argv)
{
    boost::asio::io_service io_service;
    tcp::endpoint endpoint(
        boost::asio::ip::address(boost::asio::ip::address_v4().from_string("127.0.0.1"/*argv[1]*/)), 8001);
    tcp::socket socket(io_service);
    socket.connect(endpoint);
    Packet packet(2, 9, 1, 0);
    packet.makeStringItem("gortor", 6 /* sizeof(argv[0])/sizeof(char)*/);
    packet.makeStringItem("1234", 4/*sizeof(argv[1])/sizeof(char)*/);
    std::string str((char*)packet.data(), packet.getLength());
    boost::asio::write(socket, boost::asio::buffer(str, str.length()));
    read_mes(socket);
    Packet packet1(4, 9, 2, 0);
    std::string str1((char*)packet1.data(), packet1.getLength());
    boost::asio::write(socket, boost::asio::buffer(str1, str1.length()));
    read_mes(socket);
    /*Packet packet2(3, 9, 3, 0);
    std::string str2((char*)packet2.data(), packet2.getLength());
    boost::asio::write(socket, boost::asio::buffer(str2, str2.length()));
    read_mes(socket);*/
    Packet packet3(5, 9, 4, 0);
    packet3.makeStringItem("gortor", 6 /*sizeof(argv[0]) / sizeof(char)*/);
    packet3.makeStringItem("test",  4/*sizeof(argv[3]) / sizeof(char)*/);
    std::string str3((char*)packet3.data(), packet3.getLength());
    boost::asio::write(socket, boost::asio::buffer(str3, str3.length()));
    read_mes(socket);
    read_mes(socket);
    Packet packet4(6, 9, 4, 0);
    packet4.makeStringItem("test", 4/* sizeof(argv[0]) / sizeof(char)*/);
    packet4.makeStringItem("gortor", 6/*sizeof(argv[3]) / sizeof(char)*/);
    packet4.makeStringItem("messss", 6/*sizeof(argv[4]) / sizeof(char)*/);
    std::string str4((char*)packet4.data(), packet4.getLength());
    boost::asio::write(socket, boost::asio::buffer(str4, str4.length()));
    read_mes(socket);
    Packet packet5(5, 9, 4, 0);
    packet5.makeStringItem("gortor", 6 /*sizeof(argv[0]) / sizeof(char)*/);
    packet5.makeStringItem("test", 4/*sizeof(argv[3]) / sizeof(char)*/);
    std::string str5((char*)packet5.data(), packet5.getLength());
    boost::asio::write(socket, boost::asio::buffer(str5, str5.length()));

    read_mes(socket);

    read_mes(socket);

    read_mes(socket);
    std::cout << "Hello World!\n"; 
}

// Запуск программы: CTRL+F5 или меню "Отладка" > "Запуск без отладки"
// Отладка программы: F5 или меню "Отладка" > "Запустить отладку"

// Советы по началу работы 
//   1. В окне обозревателя решений можно добавлять файлы и управлять ими.
//   2. В окне Team Explorer можно подключиться к системе управления версиями.
//   3. В окне "Выходные данные" можно просматривать выходные данные сборки и другие сообщения.
//   4. В окне "Список ошибок" можно просматривать ошибки.
//   5. Последовательно выберите пункты меню "Проект" > "Добавить новый элемент", чтобы создать файлы кода, или "Проект" > "Добавить существующий элемент", чтобы добавить в проект существующие файлы кода.
//   6. Чтобы снова открыть этот проект позже, выберите пункты меню "Файл" > "Открыть" > "Проект" и выберите SLN-файл.
