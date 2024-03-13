#ifndef POSE_ESTIMATION_RESULTS_HPP
#define POSE_ESTIMATION_RESULTS_HPP

#include <opencv2/core.hpp>

namespace flt::cvt::pose{

struct keypoint
{
    float x;
    float y;
    float score;
};

}

#endif // POSE_ESTIMATION_RESULTS_HPP
