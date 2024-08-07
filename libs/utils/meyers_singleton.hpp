#pragma once

#include <utility>

namespace flt{

template<typename T, size_t N = 0>
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
