#include "face_utils.hpp"

#include <numbers>

namespace flt::cvt::face{

double count_angle(std::vector<cv::Point2f> const &landmarks)
{
    double const a = landmarks[2].y - (landmarks[0].y + landmarks[1].y) / 2;
    double const b = landmarks[2].x - (landmarks[0].x + landmarks[1].x) / 2;
    return std::atan(std::abs(b) / a) * 180.0 / std::numbers::pi;
}

}
