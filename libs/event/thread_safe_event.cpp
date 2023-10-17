#include "thread_safe_event.hpp"

namespace ocv{

thread_safe_event::thread_safe_event()
{

}

void thread_safe_event::add_listener(listener func, void *key)
{
    //fix me : replace it with contains when the compiler support
    std::lock_guard<std::mutex> lock(mutex_);
    if(map_.find(key) == std::end(map_)){
        map_.insert({key, std::move(func)});
    }
}

void thread_safe_event::clear()
{
    std::lock_guard<std::mutex> lock(mutex_);
    map_.clear();
}

size_t thread_safe_event::size() const noexcept
{
    std::lock_guard<std::mutex> lock(mutex_);
    return map_.size();
}

void thread_safe_event::remove_listener(void *key)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if(map_.find(key) != std::end(map_)){
        map_.erase(key);
    }
}

}
