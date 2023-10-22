#ifndef MEYERS_SINGLETON_HPP
#define MEYERS_SINGLETON_HPP

template<typename T>
class meyers_singleton
{
public:
    meyers_singleton() = delete;
    meyers_singleton(meyers_singleton const&) = delete;
    meyers_singleton& operator=(meyers_singleton const&) = delete;
    meyers_singleton(meyers_singleton&&) = delete;
    meyers_singleton& operator=(meyers_singleton&&) = delete;

    static T& get_instance()
    {
        static T instance;
        return instance;
    }
};

#endif // MEYERS_SINGLETON_HPP
