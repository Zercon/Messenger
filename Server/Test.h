#pragma once
#include <thread>

class Test
{
public:
    int i = 0;
    std::shared_ptr<std::thread> t1;
    std::shared_ptr<std::thread> t2;
    Test()
    {
        t1.reset(new std::thread(&Test::work1, this));
        t1.reset(new std::thread(&Test::work2, this));
    }
    ~Test() {
        t1->join();
        t2->join();
    }
    void work1() {
        while (true) {
            i += 5;
            printf("%d work1\n", i);
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }
    void work2() {
        while (true) {
            i += 3;
            printf("%d work2\n", i);

            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }
};

