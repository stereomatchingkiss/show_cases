#ifndef POSE_ESTIMATION_UTILS_HPP_45645KF
#define POSE_ESTIMATION_UTILS_HPP_45645KF

#include "pose_estimation_results.hpp"

#include <opencv2/core.hpp>

namespace flt::cvt::pose{

void draw(cv::Mat& bgr, std::vector<keypoint> const &ktps, float min_confidence = 0.3f);

}

#endif // POSE_ESTIMATION_UTILS_HPP_45645KF
