#include "Event.h"

Event::~Event() {}

Event::Type Event::type() const
{
    return Event::Type::Null;
}

bool Event::serialize(void*& ptr, size_t& size) const
{
    ptr = malloc(1);
    if (ptr != nullptr)
    {
        size = 1;
        return true;
    }
    return false;
}

std::shared_ptr<Event> deserialize(void* buffer, size_t size)
{
    return std::make_shared<Event>();
}