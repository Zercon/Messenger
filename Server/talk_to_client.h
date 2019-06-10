#pragma once
#include "pch.h"
#include "../libs/Packet.h"

class Controller;
class talk_to_client : public boost::enable_shared_from_this<talk_to_client>
{
public:
    talk_to_client(boost::asio::io_service& service, Controller *controller): sock_(service), controller_(controller) { 
        username_ = "NONE";
        boost::posix_time::ptime last_ping = boost::posix_time::microsec_clock::local_time();
    }
    std::string username() const { return username_; }
    void answer_to_client();
    void set_clients_changed() { clients_changed_ = true; }
    boost::asio::ip::tcp::socket & sock() { return sock_; }
    bool timed_out() const;
    void stop();
    void read_request();
    void write(Packet && packet);

private:
    //function
    void process_request(Packet&& packet);
    void handle_register(Packet&& packet);
    void handle_login(Packet&& packet);
    void handle_ping(Packet&& packet);
    void handle_request_list(Packet&& packet);
    void handle_request_diologe(Packet&& packet);

    void handle_message(Packet && packet);

    void error_message(Packet && packet, uint8_t cod = 255);

    

    //std::shared_ptr<Packet> command;
    boost::asio::ip::tcp::socket sock_;
    enum { max_msg = 1024 };
    int got_bytes;
    char buff_[max_msg];
    bool started_;
    std::string username_;
    bool clients_changed_;
    boost::posix_time::ptime last_ping;

    Controller *controller_;

    //MySQL
    //std::shared_ptr<mysqlx::Session> session_mysql_;
};

