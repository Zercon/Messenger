// Server.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include "pch.h"
#include "talk_to_client.h"

typedef boost::shared_ptr<talk_to_client> client_ptr;
typedef std::vector<client_ptr> vector;
vector clients;
boost::recursive_mutex cs; // thread-safe access to clients array

void accept_thread()
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

void handle_clients_thread()
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

int main()
{
    boost::thread_group threads;
    threads.create_thread(accept_thread);
    threads.create_thread(handle_clients_thread);
    threads.join_all();
}

// Запуск программы: CTRL+F5 или меню "Отладка" > "Запуск без отладки"
// Отладка программы: F5 или меню "Отладка" > "Запустить отладку"

// Советы по началу работы 
//   1. В окне обозревателя решений можно добавлять файлы и управлять ими.
//   2. В окне Team Explorer можно подключиться к системе управления версиями.
//   3. В окне "Выходные данные" можно просматривать выходные данные сборки и другие сообщения.
//   4. В окне "Список ошибок" можно просматривать ошибки.
//   5. Последовательно выберите пункты меню "Проект" > "Добавить новый элемент", чтобы создать файлы кода, или "Проект" > "Добавить существующий элемент", чтобы добавить в проект существующие файлы кода.
//   6. Чтобы снова открыть этот проект позже, выберите пункты меню "Файл" > "Открыть" > "Проект" и выберите SLN-файл.
