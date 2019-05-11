#include "pch.h"
#include "talk_to_client.h"


void talk_to_client::answer_to_client()
{
    try
    {
        read_request();
        process_request();
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
            command->init(hdr.cod, hdr.length, hdr.ident, hdr.itemCount);
            got_bytes += boost::asio::read(sock_, boost::asio::buffer(command->payload(), hdr.length - sizeof(hdr)), boost::asio::transfer_at_least(1), ec);
            if (ec) {
                std::cout << "ERROR2: failed to read data: " << ec.message() << std::endl;
                return;
            }
            if (got_bytes == hdr.length)
            {
                process_request();
            }
        }
    }
}

void talk_to_client::process_request()
{

}
