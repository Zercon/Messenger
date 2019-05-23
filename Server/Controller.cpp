#include "pch.h"
#include "Controller.h"


Controller::Controller()
{
    link = mysql_init(0);

    if (!link)
    {
        throw std::logic_error("Невозможно получить дискриптор!\n");
    }

    if (!mysql_real_connect(link, Host, User, Pass, DBase, 3306, 0, 0))
    {
        mysql_close(link);
        throw std::logic_error("Невозможно подключиться к Базе данных!\n");
    }

}


Controller::~Controller()
{
    if (!link) {
        mysql_close(link);
    }
}

void Controller::start()
{
    boost::thread_group threads;
    threads.create_thread(boost::bind(&Controller::accept_thread,this));
    threads.create_thread(boost::bind(&Controller::handle_clients_thread, this));
    threads.join_all();
}

bool Controller::querry()
{
    return false;
}


void Controller::accept_thread()
{
    boost::asio::io_service service;
    boost::asio::ip::tcp::acceptor acceptor(service, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), 8001));
    while (true)
    {
        client_ptr new_(new talk_to_client(service));
        acceptor.accept(new_->sock());
        boost::recursive_mutex::scoped_lock lk(cs);
        clients.push_back(new_);
    }
}


void Controller::handle_clients_thread()
{
    while (true)
    {
        boost::this_thread::sleep(boost::posix_time::millisec(1));
        boost::recursive_mutex::scoped_lock lk(cs);
        for (vector::iterator b = clients.begin(), e = clients.end(); b != e; ++b)
        {
            (*b)->answer_to_client();
        }
        // erase clients that timed out
        clients.erase(std::remove_if(clients.begin(), clients.end(),
            boost::bind(&talk_to_client::timed_out, _1)), clients.end());
    }
}