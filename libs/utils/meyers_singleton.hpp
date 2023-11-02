#pragma once

namespace flt{

template<typename T>
class meyers_singleton
{
public:
    meyers_singleton() = delete;
    meyers_singleton(meyers_singleton const&) = delete;
    meyers_singleton& operator=(meyers_singleton const&) = delete;
    meyers_singleton(meyers_singleton&&) = delete;
    meyers_singleton& operator=(meyers_singleton&&) = delete;

    template<typename... U>
    static T& get_instance(U... args)
    {
        static T instance(std::forward<U>(args)...);
        return instance;
    }
};

}
