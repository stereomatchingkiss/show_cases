#pragma once

#include <opencv2/core.hpp>

namespace flt::cvt::face{

/**
 * Count angle with 5 landmarks, the order of the landmarks should be
 * left eye, nose, right eye
 * @return
 */
double count_angle(std::vector<cv::Point2f> const &landmarks);

}
