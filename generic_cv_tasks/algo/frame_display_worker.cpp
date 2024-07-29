#include "frame_display_worker.hpp"

#include <QPixmap>

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>

frame_display_worker::frame_display_worker(bool is_rtsp, QObject *parent)
    : flt::mm::frame_process_base_worker{15, parent},
    is_rtsp_{is_rtsp}
{

}

frame_display_worker::~frame_display_worker()
{

}

void frame_display_worker::process_results(std::any frame)
{
    QImage qimg;    
    if(!is_rtsp_){
        qimg = std::any_cast<QImage>(frame);
    }else{        
        auto mat = std::any_cast<cv::Mat>(frame);        
        if(mat.channels() == 3){            
            cv::cvtColor(mat, mat, cv::COLOR_BGR2RGB);
        }else if(mat.channels() == 1){            
            cv::cvtColor(mat, mat, cv::COLOR_GRAY2RGB);
        }

        qimg = QImage((uchar*) mat.data, mat.cols, mat.rows, mat.step, QImage::Format_RGB888).copy();        
    }    

    emit send_process_results(QPixmap::fromImage(std::move(qimg)));
}
