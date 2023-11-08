#pragma once

#include <QImage>
#include <QPixmap>
#include <QString>

#include <opencv2/core.hpp>

struct obj_det_worker_results
{
    bool alarm_on_ = false;
#ifndef WASM_BUILD
    cv::Mat mat_;
#else
    QImage mat_;
#endif
};
