#include "frame_capture_controller.hpp"

#include "frame_capture_base_worker.hpp"

#include <QThread>

namespace flt::mm{

frame_capture_controller::frame_capture_controller(frame_capture_base_worker *worker, QObject *parent) :
    QObject(parent),
    worker_(worker)
{
    thread_ = std::make_unique<QThread>();
    worker_->moveToThread(thread_.get());

    connect(thread_.get(), &QThread::finished, worker_, &QObject::deleteLater);
    connect(worker_, &frame_capture_base_worker::message_error, this, &frame_capture_controller::message_error);
    connect(this, &frame_capture_controller::start, worker_, &frame_capture_base_worker::start);

    thread_->start();
}

frame_capture_controller::~frame_capture_controller()
{
    stop();
    thread_->quit();
    thread_->wait();
}

void frame_capture_controller::add_listener(std::function<void (std::any)> func, void *key)
{
    worker_->add_listener(std::move(func), key);
}

void frame_capture_controller::clear()
{
    worker_->clear();
}

void frame_capture_controller::remove_listener(void *key)
{
    worker_->remove_listener(key);
}

void frame_capture_controller::stop()
{
    worker_->stop();
}

}
