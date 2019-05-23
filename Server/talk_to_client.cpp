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
    return ms > 50000;
}

void talk_to_client::stop()
{
    boost::system::error_code err; sock_.close(err);
}

void talk_to_client::read_request()
{
    if (sock_.available())
    {
        Packet command;
        boost::system::error_code ec;
        struct com {
            uint8_t cod;
            uint32_t length;
            uint16_t ident;
            uint16_t itemCount;
        } hdr;
        
        if (got_bytes += boost::asio::read(sock_, boost::asio::buffer(&hdr, sizeof(hdr)), boost::asio::transfer_at_least(1), ec)) {
            if (ec) {
                std::cout << "ERROR1: failed to read data: " << ec.message() << std::endl;
                return;
            }
            command.init(hdr.cod, hdr.length, hdr.ident, hdr.itemCount);
            got_bytes += boost::asio::read(sock_, boost::asio::buffer(command.payload(), hdr.length - sizeof(hdr)), boost::asio::transfer_at_least(1), ec);
            if (ec) {
                std::cout << "ERROR2: failed to read data: " << ec.message() << std::endl;
                return;
            }
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
    Controller::querry();
}

void talk_to_client::handle_login(Packet && packet)
{
}

void talk_to_client::handle_ping(Packet && packet)
{
}

void talk_to_client::handle_request_list(Packet && packet)
{
}

void talk_to_client::handle_request_diologe(Packet && packet)
{
}
