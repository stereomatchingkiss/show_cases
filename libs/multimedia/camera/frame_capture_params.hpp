#ifndef FRAME_CAPTURE_PARAMS_HPP
#define FRAME_CAPTURE_PARAMS_HPP

#include <string>

namespace ocv{

struct frame_capture_params
{
    int max_fps_ = 30;
    std::string url_;
};

}

#endif // FRAME_CAPTURE_PARAMS_HPP
