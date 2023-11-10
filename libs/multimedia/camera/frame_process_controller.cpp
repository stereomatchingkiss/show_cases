#include "frame_process_controller.hpp"

#include "frame_process_base_worker.hpp"

#include <QThread>

namespace flt::mm{

frame_process_controller::frame_process_controller(frame_process_base_worker *worker, QObject *parent)
    : QObject{parent},
    thread_{std::make_unique<QThread>()},
    worker_{worker}
{
    worker_->moveToThread(thread_.get());

    connect(thread_.get(), &QThread::finished, worker_, &QObject::deleteLater);
    connect(worker_, &frame_process_base_worker::send_process_results, this, &frame_process_controller::send_process_results);
    connect(this, &frame_process_controller::start, worker_, &frame_process_base_worker::start);
    connect(worker_, &frame_process_base_worker::message_error, this, &frame_process_controller::message_error);

    thread_->start();
}

frame_process_controller::~frame_process_controller()
{
    worker_->stop(true);
    thread_->quit();
    thread_->wait();
}

void frame_process_controller::predict(std::any results)
{
    worker_->predict(results);
}

void frame_process_controller::stop(bool val)
{
    worker_->stop(val);
}

}
