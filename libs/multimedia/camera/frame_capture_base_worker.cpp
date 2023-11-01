#include "frame_capture_base_worker.hpp"

#include <event/thread_safe_event.hpp>

#include "frame_capture_params.hpp"

namespace flt::mm{

struct frame_capture_base_worker::impl
{
    thread_safe_event events_;
};

frame_capture_base_worker::frame_capture_base_worker(frame_capture_params params, QObject *parent) :
    QObject(parent),
    impl_(std::make_unique<impl>())
{

}

frame_capture_base_worker::~frame_capture_base_worker()
{

}

void frame_capture_base_worker::add_listener(std::function<void (std::any)> func, void *key)
{
    impl_->events_.add_listener(std::move(func), key);
}

void frame_capture_base_worker::clear()
{
    impl_->events_.clear();
}

void frame_capture_base_worker::remove_listener(void *key)
{
    impl_->events_.remove_listener(key);
}

void frame_capture_base_worker::call_listener(std::any val, void *key)
{
    impl_->events_.call_listener(std::move(val), key);
}

void frame_capture_base_worker::call_listeners(std::any val)
{
    impl_->events_.call_listeners(std::move(val));
}

}
