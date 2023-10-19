#include "widget_stream_player.hpp"
#include "ui_widget_stream_player.h"

#include <multimedia/camera/frame_capture_controller.hpp>
#include <multimedia/camera/frame_capture_opencv_worker.hpp>
#include <multimedia/camera/frame_capture_params.hpp>
#include <multimedia/camera/frame_process_base_worker.hpp>
#include <multimedia/camera/frame_process_controller.hpp>

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>

#include <QImage>
#include <QPixmap>

widget_stream_player::widget_stream_player(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::widget_stream_player),
    frame_capture_controller_{nullptr},
    frame_capture_opencv_worker_{nullptr},
    frame_process_controller_{nullptr}
{
    ui->setupUi(this);

    ui->labelStream->setScaledContents(true);

    connect(this, &widget_stream_player::send_frame_to_display, this, &widget_stream_player::display_frame);
}

widget_stream_player::~widget_stream_player()
{
    delete ui;
}

void widget_stream_player::play(ocv::frame_capture_params const &params, ocv::frame_process_base_worker *process_worker)
{
    if(frame_capture_controller_ != nullptr){
        frame_capture_controller_->deleteLater();
        frame_process_controller_->deleteLater();
    }
    frame_capture_opencv_worker_ = new ocv::frame_capture_opencv_worker(params);
    frame_capture_controller_ = new ocv::frame_capture_controller(frame_capture_opencv_worker_, this);
    frame_process_controller_ = new ocv::frame_process_controller(process_worker, this);

    connect(frame_capture_controller_, &ocv::frame_capture_controller::message_error, this, &widget_stream_player::message_error);
    connect(frame_process_controller_, &ocv::frame_process_controller::send_frame_to_display, this, &widget_stream_player::display_frame);
    connect(frame_process_controller_, &ocv::frame_process_controller::message_error, this, &widget_stream_player::message_error);

    auto func = [=](std::any val)
    {        
        emit frame_process_controller_->process_frame(std::move(val));
    };
    frame_capture_controller_->add_listener(func, this);
    emit frame_capture_controller_->start();
}

void widget_stream_player::display_frame(QPixmap img)
{
    ui->labelStream->setPixmap(img);
}
