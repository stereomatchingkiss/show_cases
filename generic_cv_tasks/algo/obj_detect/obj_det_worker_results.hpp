#pragma once

#include <QPixmap>
#include <QString>

#include <opencv2/core.hpp>

struct obj_det_worker_results
{
    bool alarm_on_ = false;    
    cv::Mat mat_;
};
