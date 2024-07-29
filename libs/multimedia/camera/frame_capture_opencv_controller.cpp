#include "frame_capture_opencv_controller.hpp"

#include "frame_capture_opencv_worker.hpp"

#include <QThread>

namespace flt::mm{

frame_capture_opencv_controller::frame_capture_opencv_controller(frame_capture_opencv_worker *worker, QObject *parent) :
    QObject{parent},
    thread_{std::make_unique<QThread>()},
    worker_{worker}
{
    worker_->moveToThread(thread_.get());

    connect(thread_.get(), &QThread::finished, worker_, &QObject::deleteLater);
    connect(worker_, &frame_capture_opencv_worker::send_process_results, this, &frame_capture_opencv_controller::send_process_results);
    connect(this, &frame_capture_opencv_controller::start, worker_, &frame_capture_opencv_worker::start);
    connect(worker_, &frame_capture_opencv_worker::message_error, this, &frame_capture_opencv_controller::message_error);

    thread_->start();
}

frame_capture_opencv_controller::~frame_capture_opencv_controller()
{
    worker_->stop(true);
    thread_->quit();
    thread_->wait();
}

void frame_capture_opencv_controller::stop(bool val)
{
    worker_->stop(val);
}



}
