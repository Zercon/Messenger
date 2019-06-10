#include "Controller.h"


Controller::Controller()
{
    /*link = mysql_init(0);
    if (!link)
    {
        throw std::logic_error("Error: Невозможно получить дескриптор подключения к базе данных!");
    }
    if (!mysql_real_connect(link, "localhost", "root", "12345", "messeneger", 0, 0, 0))
    {
        throw std::logic_error("Невозможно подключиться к базе данных!");
    }*/
}


Controller::~Controller()
{
    /*mysql_close(link);*/
}

void Controller::start()
{
    boost::thread_group threads;
    threads.create_thread(boost::bind(&Controller::accept_thread,this));
    threads.create_thread(boost::bind(&Controller::handle_clients_thread, this));
    threads.join_all();
}
//
//bool Controller::query(char* data, int len)
//{
//    std::string query_str(data, len);
//    MYSQL_RES *result = 0;
//    MYSQL_ROW row;
//
//    std::cout << "Query: " << query_str << std::endl;
//
//    if (mysql_query(link, "SET NAMES 'cp1251'") != 0)
//    {
//        mysql_close(link);
//        throw std::logic_error("Error: Невозможно провести запрос!");
//    }
//
//    if (mysql_query(link, "use messeneger;") != 0)
//    {
//        mysql_close(link);
//        throw std::logic_error("Error: Невозможно провести запрос!");
//    }
//
//    if (mysql_query(link, query_str.c_str()))
//        // c_str() для string возвращает char-строку
//    {
//        throw std::logic_error("Невозможно провести запрос!");
//    }
//
//    // Извлечение результата
//    result = mysql_store_result(link);
//    if (result)
//    {
//        
//        //throw std::logic_error("Извлечение результатов запроса завершилось аварийно!");
//        {
//            while (row = mysql_fetch_row(result))
//            {
//                for (int i = 0; i < mysql_num_fields(result); i++)
//                {
//                    std::cout << row[i] << std::endl;
//                }
//                
//            }
//        }
//    }
//    else
//    {
//        std::cout << "Не найдено ни одной статьи этого автора" << std::endl;
//    }
//    mysql_free_result(result);
//    return false;
//}
//
//bool Controller::query_login(char* data, int len)
//{
//    std::string query_str(data, len);
//    MYSQL_RES *result = 0;
//    MYSQL_ROW row;
//
//    std::cout << "Query: " << query_str << std::endl;
//
//    if (mysql_query(link, "SET NAMES 'cp1251'") != 0)
//    {
//        mysql_close(link);
//        throw std::logic_error("Error: Невозможно провести запрос!");
//    }
//
//    if (mysql_query(link, "use messeneger;") != 0)
//    {
//        mysql_close(link);
//        throw std::logic_error("Error: Невозможно провести запрос!");
//    }
//
//    if (mysql_query(link, query_str.c_str()))
//        // c_str() для string возвращает char-строку
//    {
//        throw std::logic_error("Невозможно провести запрос!");
//    }
//
//    // Извлечение результата
//    result = mysql_store_result(link);
//    if (result)
//    {
//        mysql_free_result(result);
//        return true;
//    }
//    else
//    {
//        std::cout << "Не найдено ни одной статьи этого автора" << std::endl;
//    }
//    return false;
//}
//
//MYSQL_RES* Controller::query_list(char* data, int len)
//{
//    std::string query_str(data, len);
//    MYSQL_RES *result = 0;
//    MYSQL_ROW row;
//
//    std::cout << "Query: " << query_str << std::endl;
//
//    if (mysql_query(link, "SET NAMES 'cp1251'") != 0)
//    {
//        mysql_close(link);
//        throw std::logic_error("Error: Невозможно провести запрос!");
//    }
//
//    if (mysql_query(link, "use messeneger;") != 0)
//    {
//        mysql_close(link);
//        throw std::logic_error("Error: Невозможно провести запрос!");
//    }
//
//    if (mysql_query(link, query_str.c_str()))
//        // c_str() для string возвращает char-строку
//    {
//        throw std::logic_error("Невозможно провести запрос!");
//    }
//
//    // Извлечение результата
//    result = mysql_store_result(link);
//    if (result)
//    {
//        return result;
//    }
//    else
//    {
//        return 0;
//        std::cout << "Не найдено ни одной статьи этого автора" << std::endl;
//    }
//    return 0;
//}
//
//MYSQL_RES* Controller::query_dial(char* data, int len)
//{
//    std::string query_str(data, len);
//    MYSQL_RES *result = 0;
//    MYSQL_ROW row;
//
//    std::cout << "Query: " << query_str << std::endl;
//
//    if (mysql_query(link, "SET NAMES 'cp1251'") != 0)
//    {
//        mysql_close(link);
//        throw std::logic_error("Error: Невозможно провести запрос!");
//    }
//
//    if (mysql_query(link, "use messeneger;") != 0)
//    {
//        mysql_close(link);
//        throw std::logic_error("Error: Невозможно провести запрос!");
//    }
//
//    if (mysql_query(link, query_str.c_str()))
//        // c_str() для string возвращает char-строку
//    {
//        throw std::logic_error("Невозможно провести запрос!");
//    }
//
//    // Извлечение результата
//    result = mysql_store_result(link);
//    if (result)
//    {
//        return result;
//    }
//    else
//    {
//        return 0;
//        std::cout << "Не найдено ни одной статьи этого автора" << std::endl;
//    }
//    return 0;
//}
//

void Controller::accept_thread()
{
    boost::asio::io_service service;
    boost::asio::ip::tcp::acceptor acceptor(service, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), 8001));
    while (true)
    {
        client_ptr new_(new talk_to_client(service, this));
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

void Controller::write_mes_to(Packet&& packet)
{
    std::string to(packet.getStringItem(1), packet.getLengthItem(1));
    for (vector::iterator b = clients.begin(), e = clients.end(); b != e; ++b)
    {
        if ((*b)->username().compare(to))
        {
            (*b)->write(std::move(packet));
        }
    }
}