#ifndef FRAME_CAPTURE_PARAMS_HPP
#define FRAME_CAPTURE_PARAMS_HPP

#include "../stream_enum.hpp"

#include <string>

namespace ocv{

struct frame_capture_params
{
    int max_fps_ = 30;
    stream_receiver_type receiver_type_;
    stream_source_type src_type_;    
    std::string url_;
};

}

#endif // FRAME_CAPTURE_PARAMS_HPP
