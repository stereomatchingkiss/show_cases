#include "frame_display_worker.hpp"

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>

#include <QPixmap>

frame_display_worker::frame_display_worker(QObject *parent)
    : ocv::mm::frame_process_base_worker{parent}
{

}

frame_display_worker::~frame_display_worker()
{

}

void frame_display_worker::process_results(std::any frame)
{
    auto mat = std::any_cast<cv::Mat>(frame);
    if(mat.channels() == 3){
        cv::cvtColor(mat, mat, cv::COLOR_BGR2RGB);
    }else{
        cv::cvtColor(mat, mat, cv::COLOR_GRAY2RGB);
    }
    auto qimg = QImage(mat.data, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);
    emit send_process_results(QPixmap::fromImage(std::move(qimg)));
}
