#pragma once

#include <string>

namespace flt::mm{

struct frame_capture_opencv_params
{
    int max_fps_ = 30;
    std::string url_;
};

}
