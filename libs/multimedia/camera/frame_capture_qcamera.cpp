#include "frame_capture_qcamera.hpp"

#include "frame_capture_qcamera_params.hpp"
#include "frame_process_controller.hpp"

#include <QCamera>
#include <QMediaCaptureSession>
#include <QTime>
#include <QVideoFrame>
#include <QVideoSink>

namespace flt::mm{

struct frame_capture_qcamera::impl
{
    impl(frame_capture_qcamera_params const &params) :
        params_{params}
    {
        cam_.setCameraDevice(params.device_);
        cam_.setFocusMode(QCamera::FocusModeAuto);

        session_.setCamera(&cam_);
        session_.setVideoSink(&sink_);

        valid_duration_ = 1000 / params.max_fps_;
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

    QCamera cam_;
    std::vector<std::pair<std::shared_ptr<frame_process_controller>, void*>> controllers_;
    QTime last_time_;
    frame_capture_qcamera_params params_;
    int valid_duration_ = 0;

    QMediaCaptureSession session_;
    QVideoSink sink_;
};

frame_capture_qcamera::frame_capture_qcamera(frame_capture_qcamera_params const &params, QObject *parent) :
    single_frame_with_multi_worker_base{parent},
    impl_{std::make_unique<impl>(params)}
{
    connect(&impl_->sink_, &QVideoSink::videoFrameChanged, this, &frame_capture_qcamera::video_frame_changed);
}

frame_capture_qcamera::~frame_capture_qcamera()
{

}

void frame_capture_qcamera::add_listener(std::shared_ptr<frame_process_controller> process_controller, void *key)
{
    impl_->controllers_.emplace_back(process_controller, key);
}

void frame_capture_qcamera::remove_listener(void *key)
{
    impl_->remove(key);
}

void frame_capture_qcamera::start()
{
    impl_->last_time_ = QTime::currentTime();
    impl_->cam_.start();
}

void frame_capture_qcamera::video_frame_changed(const QVideoFrame &frame)
{
    auto const ctime = QTime::currentTime();
    if(impl_->last_time_.msecsTo(ctime) > impl_->valid_duration_){
        impl_->last_time_ = ctime;
        if(auto mat = frame.toImage(); !mat.isNull()){
            for(auto &val : impl_->controllers_){
                emit val.first->process_results(mat);
            }
        }else{
            qDebug()<<__func__<<":cannot decode video frame of webcam";
        }
    }
}



}
