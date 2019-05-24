#pragma once
#include "pch.h"
#include "../libs/Packet.h"


class talk_to_client : public boost::enable_shared_from_this<talk_to_client>
{
public:
    talk_to_client(boost::asio::io_service& service): sock_(service) { 
        
    }
    std::string username() const { return username_; }
    void answer_to_client();
    void set_clients_changed() { clients_changed_ = true; }
    boost::asio::ip::tcp::socket & sock() { return sock_; }
    bool timed_out() const;
    void stop();
    void read_request();

private:
    //function
    void process_request(Packet&& packet);
    void handle_register(Packet&& packet);
    void handle_login(Packet&& packet);
    void handle_ping(Packet&& packet);
    void handle_request_list(Packet&& packet);
    void handle_request_diologe(Packet&& packet);

    //std::shared_ptr<Packet> command;
    boost::asio::ip::tcp::socket sock_;
    enum { max_msg = 1024 };
    int got_bytes;
    char buff_[max_msg];
    bool started_;
    std::string username_;
    bool clients_changed_;
    boost::posix_time::ptime last_ping;

    //MySQL
    //std::shared_ptr<mysqlx::Session> session_mysql_;
};

