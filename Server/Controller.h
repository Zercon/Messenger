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

    bool query(char*, int);

    bool query_login(char * data, int len);

    MYSQL_RES * query_list(char * data, int len);

    MYSQL_RES * query_dial(char * data, int len);

private:
    void accept_thread();
    void handle_clients_thread();

    MYSQL *link;

};

