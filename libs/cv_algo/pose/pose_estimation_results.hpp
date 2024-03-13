#ifndef POSE_ESTIMATION_RESULTS_HPP
#define POSE_ESTIMATION_RESULTS_HPP

#include <opencv2/core.hpp>

namespace flt::cvt::pose{

struct pose_estimation_results
{
    std::vector<std::vector<float>> confidences_;
    std::vector<std::vector<cv::Point>> keypoints_;    
    std::vector<cv::Rect> rects_;
};

}

#endif // POSE_ESTIMATION_RESULTS_HPP
