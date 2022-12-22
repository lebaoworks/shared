#pragma once

#include <condition_variable>
#include <list>
#include <memory>
#include <mutex>
#include <thread>

#include "Event.h"

class BufferQueue
{
    static const size_t capacity = 1000;
public:
    BufferQueue();
    ~BufferQueue();

    /// @brief Push event in to queue
    /// @param event
    /// @exception thow exception in case event is null
    void push(Event* event);

    /// @brief Get even
    /// @return pointer to event if any.
    Event* pop();

private:
    bool _exit = true;

    std::mutex _events_mutex;
    std::list<std::unique_ptr<Event>> _events;
    
    std::mutex _push_mutex;
    std::condition_variable _push_signal;
    std::thread _save_worker;
    void save_job();
    
    std::mutex _pop_mutex;
    std::condition_variable _pop_signal;
    std::thread _restore_worker;
    void restore_job();
};