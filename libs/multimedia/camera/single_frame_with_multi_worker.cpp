#include "single_frame_with_multi_worker.hpp"

#include "frame_capture_base_worker.hpp"
#include "frame_capture_controller.hpp"
#include "frame_capture_opencv_worker.hpp"
#include "frame_capture_params.hpp"

#include "frame_process_controller.hpp"

#include <multimedia/camera/single_frame_with_multi_worker.hpp>

namespace flt::mm{


single_frame_with_multi_worker::single_frame_with_multi_worker(frame_capture_params const &params,
                                                               QObject *parent) :
    QObject{parent}
{
    if(params.receiver_type_ == stream_receiver_type::opencv){
        frame_capture_base_worker *worker = new frame_capture_opencv_worker(params);
        frame_capture_controller_ = std::make_unique<frame_capture_controller>(worker);

        connect(frame_capture_controller_.get(), &frame_capture_controller::message_error,
                this, &single_frame_with_multi_worker::message_error);
    }else{
        throw std::runtime_error("Only support opencv");
    }
}

single_frame_with_multi_worker::~single_frame_with_multi_worker()
{    
    stop();
}

void single_frame_with_multi_worker::add_listener(std::shared_ptr<frame_process_controller> process_controller, void *key)
{    
    frame_capture_controller_->add_listener([process_controller](std::any input)
    {        
        process_controller->process_results(input);
    }, key);
    frame_process_vector_.emplace_back(process_controller, key);    
}

void single_frame_with_multi_worker::remove_listener(void *key)
{
    frame_capture_controller_->remove_listener(key);
    for(auto it = std::begin(frame_process_vector_); it != std::end(frame_process_vector_); ++it){
        if(it->second == key){
            frame_process_vector_.erase(it);
            break;
        }
    }
}

void single_frame_with_multi_worker::start()
{
    emit frame_capture_controller_->start();
}

void single_frame_with_multi_worker::stop()
{
    if(frame_capture_controller_){
        //warning : must reset the frame_capture_controller first, because this will make sure
        //every frame process do not need to be called in another thread
        frame_capture_controller_.reset();
        //Now it is safe to release the memory, because all of the frame process are released from another threads
        frame_process_vector_.clear();
    }
}

}
