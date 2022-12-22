#pragma once

#include <cstddef>
#include <memory>

struct Event
{
    enum Type {
        Null,
    };

    virtual ~Event();

    virtual Event::Type type() const;

    /// @brief serialize event to binary data.
    /// @param ptr pointer to serialized data. Use free() to deallocates data.
    /// @param size size of serialized data.
    /// @return true if serializing success, false otherwise.
    virtual bool serialize(void*& ptr, size_t& size) const;

    /// @brief deserialize event from buffer.
    /// @param buffer pointer serialized data.
    /// @param size size of serialized data. 
    /// @return pointer to event, or null if deserializing failed.
    static std::shared_ptr<Event> deserialize(void* buffer, size_t size);
};
