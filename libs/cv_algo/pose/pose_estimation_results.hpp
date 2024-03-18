#pragma once

#include <opencv2/core.hpp>

namespace flt::cvt::pose{

struct keypoint
{
    int x_;
    int y_;
    int hx_; //h == heatmap
    int hy_;
    float score_;
};

}
