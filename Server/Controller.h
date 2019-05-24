#pragma once

#include "pch.h"
#include "talk_to_client.h"


class Controller
{
    typedef boost::shared_ptr<talk_to_client> client_ptr;
    typedef std::vector<client_ptr> vector;
    vector clients;
    boost::recursive_mutex cs; // thread-safe access to clients array
public:
    Controller();
    ~Controller();
    void start();

    bool query(std::string&&);

private:
    void accept_thread();
    void handle_clients_thread();

    MYSQL *link;

};

