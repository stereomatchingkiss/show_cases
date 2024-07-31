#include "frame_capture_opencv.hpp"

#include "frame_capture_opencv_params.hpp"
#include "frame_process_controller.hpp"

#include "frame_capture_opencv_controller.hpp"
#include "frame_capture_opencv_worker.hpp"

#include <opencv2/highgui.hpp>
#include <opencv2/videoio.hpp>

namespace flt::mm{

struct frame_capture_opencv::impl
{
    impl(frame_capture_opencv_params params) :
        params_{std::move(params)}
    {                
    }

    void remove(void *key)
    {
        auto func = [key](auto const &val)
        {
            return key == val.second;
        };
        if(auto it = std::ranges::find_if(controllers_, func); it != std::end(controllers_)){
            controllers_.erase(it);
        }
    }

    std::vector<std::pair<std::shared_ptr<frame_process_controller>, void*>> controllers_;
    frame_capture_opencv_params params_;    

    frame_capture_opencv_controller *opencv_controller_;
    frame_capture_opencv_worker *opencv_worker_;
};

frame_capture_opencv::frame_capture_opencv(frame_capture_opencv_params params, QObject *parent) :
    single_frame_with_multi_worker_base(parent),
    impl_(std::make_unique<impl>(std::move(params)))
{
    impl_->opencv_worker_ = new frame_capture_opencv_worker(impl_->params_);
    impl_->opencv_controller_ = new frame_capture_opencv_controller(impl_->opencv_worker_, this);

    connect(impl_->opencv_controller_, &frame_capture_opencv_controller::send_process_results,
            this, &frame_capture_opencv::send_process_results);
}

frame_capture_opencv::~frame_capture_opencv()
{        
}

void frame_capture_opencv::add_listener(std::shared_ptr<frame_process_controller> process_controller, void *key)
{
    impl_->controllers_.emplace_back(process_controller, key);
}

void frame_capture_opencv::remove_listener(void *key)
{
    impl_->remove(key);
}

void frame_capture_opencv::start()
{
    emit impl_->opencv_controller_->start();
}

void frame_capture_opencv::send_process_results(std::any frame)
{
    for(auto &val : impl_->controllers_){
        val.first->predict(frame);
    }
}


}
