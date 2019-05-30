#include "pch.h"
#include "talk_to_client.h"
#include "Controller.h"

void talk_to_client::answer_to_client()
{
    try
    {
        read_request();
        //process_request();
    }
    catch (boost::system::system_error&)
    {
        stop();
    }
    if (timed_out())
        stop();
}

bool talk_to_client::timed_out() const
{
    boost::posix_time::ptime now = boost::posix_time::microsec_clock::local_time();
    long long ms = (now - last_ping).total_milliseconds();
    return ms > 500000;
}

void talk_to_client::stop()
{
    boost::system::error_code err; sock_.close(err);
}

void talk_to_client::read_request()
{
    if (sock_.available())
    {
        got_bytes = 0;
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
            got_bytes += boost::asio::read(sock_, boost::asio::buffer(command.payload(), hdr.length - sizeof(hdr)), boost::asio::transfer_at_least(1), ec);
            if (ec) {
                std::cout << "ERROR2: failed to read data: " << ec.message() << std::endl;
                return;
            }
            for (int i = 0; i < got_bytes; i++) {
                printf("%02X ", *(command.data() + i));
            }
            printf("\n");
            if (got_bytes == hdr.length)
            {
                process_request(std::move(command));
            }
        }
    }
}

void talk_to_client::process_request(Packet&& packet)
{
    last_ping = boost::posix_time::microsec_clock::local_time();
    if ((int)packet.getCod() == 1)
    {
        handle_register(std::move(packet));
    }
    else if ((int)packet.getCod() == 2)
    {
        handle_login(std::move(packet));
    }
    else if ((int)packet.getCod() == 3)
    {
        handle_ping(std::move(packet));
    }
    else if ((int)packet.getCod() == 4)
    {
        handle_request_list(std::move(packet));
    }
    else if ((int)packet.getCod() == 5)
    {
        handle_request_diologe(std::move(packet));
    }
    else
    {
        std::cout << "Error: invalid cod msg!" << std::endl;
    }
}

void talk_to_client::handle_register(Packet && packet)
{
    uint32_t len = *((uint32_t*)(packet.data() + 9 + 1));
    std::string login((char*)packet.data() + 9 + 5, len - 5);
    std::string pwd((char*)packet.data() + 9 + len + 5, *((uint32_t*)(packet.data() + 9 + len + 1)) - 5);
    std::string query = "INSERT INTO messeneger.users (login, password) VALUES ('" + login +"', '" + pwd +"');";
    //std::string query = "SELECT * FROM messeneger.users;";
    try {
        controller_->query((char*)query.c_str(), query.length());
    }
    catch (std::exception e) {
        error_message(std::move(packet));
    }
}

void talk_to_client::handle_login(Packet && packet)
{
    uint32_t len = *((uint32_t*)(packet.data() + 9 + 1));
    std::string login((char*)packet.data() + 9 + 5, len - 5);
    std::string pwd((char*)packet.data() + 9 + len + 5, *((uint32_t*)(packet.data() + 9 + len + 1)) - 5);
    std::string query = "SELECT * FROM messeneger.users WHERE login = '" + login + "' AND password = '" + pwd + "';";
    //std::string query = "SELECT * FROM messeneger.users;";
    try {
        if (!controller_->query_login((char*)query.c_str(), query.length())) {
            error_message(std::move(packet), 2);
        }
        else
        {
            error_message(std::move(packet), 1);
        }
    }
    catch (std::exception e) {
        error_message(std::move(packet));
    }
}

void talk_to_client::handle_ping(Packet && packet)
{
    write(std::move(packet));
}

void talk_to_client::handle_request_list(Packet && packet)
{
    std::string query = "SELECT * FROM messeneger.users;";
    try {
        MYSQL_RES * res = controller_->query_list((char*)query.c_str(), query.length());
        Packet ans(packet.getCod() + 128, 9, packet.getIdent(), 0);
        if (res)
        {
            
            MYSQL_ROW row;
            while (row = mysql_fetch_row(res))
            {
               // for (int i = 0; i < mysql_num_fields(res); i++)
                //{
                //    std::cout << row[i] << std::endl;
                ans.makeStringItem(row[0], sizeof(row[0]) / sizeof(char));
                //}

            }


        }
        write(std::move(ans));
    }
    catch (std::exception e) {
        error_message(std::move(packet));
    }
}

void talk_to_client::handle_request_diologe(Packet && packet)
{

    uint32_t len = *((uint32_t*)(packet.data() + 9 + 1));
    std::string login((char*)packet.data() + 9 + 5, len - 5);
    std::string pwd((char*)packet.data() + 9 + len + 5, *((uint32_t*)(packet.data() + 9 + len + 1)) - 5);
    std::string query = "SELECT * FROM messeneger.messeges WHERE to_user = '" + login + "' AND from_user = '" + pwd + "';";
    //std::string query = "SELECT * FROM messeneger.users;";
    try {
        MYSQL_RES * res = controller_->query_dial((char*)query.c_str(), query.length());
        Packet ans(packet.getCod() + 128, 9, packet.getIdent(), 0);
        if (res)
        {
            ans.makeCodItem(1);
            write(std::move(ans));
            MYSQL_ROW row;
            while (row = mysql_fetch_row(res))
            {
                Packet anss(packet.getCod() + 128, 9, packet.getIdent(), 0);
                anss.makeStringItem(row[2], sizeof(row[2]) / sizeof(char));
                anss.makeStringItem(row[4], sizeof(row[4]) / sizeof(char));
                write(std::move(packet));
            }


        }
        write(std::move(ans));
    }
    catch (std::exception e) {
        error_message(std::move(packet));
    }
}

void talk_to_client::error_message(Packet && packet, uint8_t cod)
{
    Packet ans(packet.getCod() + 128, 9, packet.getIdent(), 0);
    ans.makeCodItem(cod);
    write(std::move(ans));
}

void talk_to_client::write(Packet && packet) {
    std::string str((char*)packet.data(), packet.getLength());
    boost::asio::write(sock_, boost::asio::buffer(str, str.length()));
}