#pragma once

#include <opencv2/core.hpp>

#include <QString>

struct anpr_algo_predict_results
{
    QString plate_num_;
    cv::Rect plate_rect_;
    cv::Rect vehicle_rect_;
};
