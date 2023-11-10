#include "frame_process_base_worker.hpp"

#include <QThread>

namespace flt::mm{

frame_process_base_worker::frame_process_base_worker(int buffer_size, QObject *parent)
    : QObject{parent},
    circular_buffer_(buffer_size),
    stop_{false}
{

}

frame_process_base_worker::~frame_process_base_worker()
{

}

void frame_process_base_worker::predict(std::any results)
{
    std::lock_guard<std::mutex> lock(mutex_);
    circular_buffer_.push_back(results);
}

void frame_process_base_worker::stop(bool val)
{
    stop_ = val;
}

void frame_process_base_worker::start()
{
    while(!stop_){
        if(auto val = pop_front(); val.has_value()){
            process_results(val);
        }else{
            QThread::msleep(10);
        }
    }
}

std::any frame_process_base_worker::pop_front()
{
    std::lock_guard<std::mutex> lock(mutex_);
    if(!circular_buffer_.empty()){
        auto val = circular_buffer_.front();
        circular_buffer_.pop_front();
        return val;
    }else{
        return {};
    }
}

}
