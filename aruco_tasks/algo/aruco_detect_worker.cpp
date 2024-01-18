#include "aruco_detect_worker.hpp"

#include <QDebug>

#include <opencv2/aruco.hpp>

struct aruco_detect_worker::impl
{

};

aruco_detect_worker::aruco_detect_worker(QObject *parent) :
    flt::mm::frame_process_base_worker(2, parent)
{

}

aruco_detect_worker::~aruco_detect_worker()
{

}

void aruco_detect_worker::process_results(std::any frame)
{    
    auto qimg = std::any_cast<QImage>(frame);    

    //emit send_process_results(impl_->predict(qimg));
}
