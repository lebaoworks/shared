#include "BufferQueue.h"

#include <iostream>
#include <stdexcept>

BufferQueue::BufferQueue()
{
    _exit = false;
    try
    {
        _save_worker = std::thread(&BufferQueue::save_job, this);
        _restore_worker = std::thread(&BufferQueue::restore_job, this);
    }
    catch (std::exception& e)
    {
        _exit = true;
        _push_signal.notify_one();
        _pop_signal.notify_one();
        if (_save_worker.joinable())
            _save_worker.join();
        if (_restore_worker.joinable())
            _restore_worker.join();
        throw;
    }
    
}

BufferQueue::~BufferQueue()
{
    _exit = true;
    _push_signal.notify_one();
    _pop_signal.notify_one();
    _save_worker.join();
    _restore_worker.join();
}

void BufferQueue::push(Event* event)
{
    if (event == nullptr)
        throw std::runtime_error("event == null");
    std::lock_guard<std::mutex> lk(_events_mutex);
    _events.push_back(std::move(std::unique_ptr<Event>(event)));
    _push_signal.notify_one();
}

Event* BufferQueue::pop()
{
    std::lock_guard<std::mutex> lk(_events_mutex);
    _pop_signal.notify_one();
    if (_events.size() > 0)
    {
        auto ret = _events.front().release();
        _events.pop_front();
        return ret;
    }
    return nullptr;
}

void BufferQueue::save_job()
{
    while (true)
    {
        std::unique_lock<std::mutex> lk(_push_mutex);
        _push_signal.wait(lk, [&]{ return _events.size() > capacity || _exit; });
        lk.unlock();

        if (_exit)
            return;

        // Do save events
        {
            std::lock_guard<std::mutex> l(_events_mutex);
            while (_events.size() > capacity*2/3)
                _events.pop_front();
            printf("%s() -> size: %d\n", __FUNCTION__, _events.size());
        }
    }
}

void BufferQueue::restore_job()
{
    while (true)
    {
        std::unique_lock<std::mutex> lk(_pop_mutex);
        _pop_signal.wait(lk, [&]{ return _events.size() < capacity/3 || _exit; });
        lk.unlock();
        
        if (_exit)
            return;

        // Do restore events
        {
            std::lock_guard<std::mutex> l(_events_mutex);
            while (_events.size() < capacity/3)
                _events.push_back(std::move(std::make_unique<Event>()));
            printf("%s() -> size: %d\n", __FUNCTION__, _events.size());
        }
    }
}

