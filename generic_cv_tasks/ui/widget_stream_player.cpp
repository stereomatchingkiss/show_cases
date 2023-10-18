#include "widget_stream_player.hpp"
#include "ui_widget_stream_player.h"

#include <multimedia/camera/frame_capture_controller.hpp>
#include <multimedia/camera/frame_capture_opencv_worker.hpp>
#include <multimedia/camera/frame_capture_params.hpp>

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>

#include <QImage>
#include <QPixmap>

widget_stream_player::widget_stream_player(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::widget_stream_player),
    frame_capture_controller_(nullptr),
    frame_capture_opencv_worker_(nullptr)
{
    ui->setupUi(this);

    ui->labelStream->setScaledContents(true);

    connect(this, &widget_stream_player::send_frame_to_display, this, &widget_stream_player::display_frame);
}

widget_stream_player::~widget_stream_player()
{

    delete ui;
}

void widget_stream_player::play(const QString &url, ocv::stream_source_type)
{
    ocv::frame_capture_params params;
    params.max_fps_ = 30;
    params.url_ = "D:/programming/videos/mode_0_0.mp4"; //url.toStdString();
    frame_capture_opencv_worker_ = new ocv::frame_capture_opencv_worker(params);
    frame_capture_controller_ = new ocv::frame_capture_controller(frame_capture_opencv_worker_, this);

    auto func = [=](std::any val)
    {
        qDebug()<<"listener called";
        auto mat = std::any_cast<cv::Mat>(val);
        if(mat.channels() == 3){
            cv::cvtColor(mat, mat, cv::COLOR_BGR2RGB);
        }else{
            cv::cvtColor(mat, mat, cv::COLOR_GRAY2RGB);
        }
        auto qimg = QImage(mat.data, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);
        emit send_frame_to_display(QPixmap::fromImage(std::move(qimg)));
    };
    frame_capture_controller_->add_listener(func, this);
    emit frame_capture_controller_->start();
}

void widget_stream_player::display_frame(QPixmap img)
{
    ui->labelStream->setPixmap(img);
}
