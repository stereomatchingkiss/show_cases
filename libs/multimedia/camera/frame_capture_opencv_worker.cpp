#include "frame_capture_opencv_worker.hpp"

#include <opencv2/highgui.hpp>
#include <opencv2/videoio.hpp>

#include <chrono>
#include <thread>

#include "frame_capture_params.hpp"

namespace ocv{

struct frame_capture_opencv_worker::impl
{
    impl(frame_capture_params params) :
        params_(std::move(params))
    {

    }

    bool open_cam() noexcept
    {
        try{            
            return cap_.open(std::stoi(params_.url_));
        }catch(std::exception const &){            
            return cap_.open(params_.url_);
        }
    }

    cv::VideoCapture cap_;
    frame_capture_params params_;
    std::atomic<bool> stop_ = true;
};

frame_capture_opencv_worker::frame_capture_opencv_worker(frame_capture_params params, QObject *parent) :
    frame_capture_base_worker({}, parent),
    impl_(std::make_unique<impl>(std::move(params)))
{

}

frame_capture_opencv_worker::~frame_capture_opencv_worker()
{

}

void frame_capture_opencv_worker::start()
{
    if(!impl_->stop_){
        return;
    }
    impl_->stop_ = false;
    if(impl_->open_cam()){
        auto const duration = std::chrono::milliseconds(1000/impl_->params_.max_fps_);
        while(!impl_->stop_){
            cv::Mat frame;            
            if(impl_->cap_>>frame; !frame.empty()){                
                call_listeners(frame);
                std::this_thread::sleep_for(duration);
            }
        }
    }
}

void frame_capture_opencv_worker::stop()
{
    impl_->stop_ = true;
}

}
