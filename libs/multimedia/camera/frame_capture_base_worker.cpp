#include "frame_capture_base_worker.hpp"

#include <event/thread_safe_event.hpp>

namespace ocv{

struct frame_capture_base_worker::impl
{
    thread_safe_event events_;
};

frame_capture_base_worker::frame_capture_base_worker(QObject *parent) :
    QObject(parent),
    impl_(std::make_unique<impl>())
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

}
