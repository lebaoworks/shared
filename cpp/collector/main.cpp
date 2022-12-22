#include <iostream>
#include <thread>
#include <chrono>

#include "Event.h"
#include "BufferQueue.h"

bool is_exit = false;

void producer(BufferQueue* queue)
{
    int count = 0;
    while (!is_exit)
    {
        queue->push(new Event());
        count++;
        std::this_thread::sleep_for(std::chrono::milliseconds(1) );
    }
    printf("%s() count: %d\n", __FUNCTION__, count);
}

void consumer(BufferQueue* queue)
{
    int count = 0;
    while (!is_exit)
    {
        auto event = queue->pop();
        if (event != nullptr)
        {
            count++;
            delete event;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1000) );
    }
    printf("%s() count: %d\n", __FUNCTION__, count);
}
int main()
{
    BufferQueue queue;
    auto t1 = std::thread(producer, &queue);
    auto t2 = std::thread(consumer, &queue);
    
    std::this_thread::sleep_for(std::chrono::seconds(5) );
    is_exit = true;
    t1.join();
    t2.join();

    printf("Done!");
    return 0;
}