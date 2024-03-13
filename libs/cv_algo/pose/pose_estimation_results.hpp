#ifndef POSE_ESTIMATION_RESULTS_HPP
#define POSE_ESTIMATION_RESULTS_HPP

#include <opencv2/core.hpp>

namespace flt::cvt::pose{

struct keypoint
{
    int x_;
    int y_;
    float score_;
};

}

#endif // POSE_ESTIMATION_RESULTS_HPP
