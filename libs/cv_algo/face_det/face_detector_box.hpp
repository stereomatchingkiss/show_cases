#pragma once

#include <opencv2/core.hpp>

#include <string>

namespace flt::cvt::face{

struct face_detector_box
{
    float confidence_;
    std::vector<cv::Point2f> landmark_pts_;
    cv::Rect rect_;    
};

}
