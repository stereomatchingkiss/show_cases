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
      connect(this, &frame_process_controller::process_results, worker_, &frame_process_base_worker::process_results);
      connect(worker_, &frame_process_base_worker::send_process_results, this, &frame_process_controller::send_process_results);
      connect(worker_, &frame_process_base_worker::message_error, this, &frame_process_controller::message_error);

      thread_->start();
}

frame_process_controller::~frame_process_controller()
{
    thread_->quit();
    thread_->wait();
}

}
