#ifndef THREAD_SAFE_EVENT_HPP_1093434
#define THREAD_SAFE_EVENT_HPP_1093434

#include <any>
#include <functional>
#include <map>
#include <mutex>

namespace ocv{

class thread_safe_event
{
public:
    using listener = std::function<void(std::any)>;

    thread_safe_event();

    void add_listener(listener func, void *key);
    void call_listener(std::any val, void *key);
    void call_listeners(std::any val);
    void clear();
    size_t size() const noexcept;
    void remove_listener(void *key);    

private:    
    std::map<void*, listener> map_;
    mutable std::mutex mutex_;
};

}

#endif
