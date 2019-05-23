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

    static bool querry();

private:
    void accept_thread();
    void handle_clients_thread();

    // For MySQL
    MYSQL *link;
    MYSQL_RES *result = 0;
    MYSQL_ROW row;

    const char* Host = "localhost";
    const char* User = "root";
    const char* Pass = "12345";
    const char* DBase = "messeneger";
};

