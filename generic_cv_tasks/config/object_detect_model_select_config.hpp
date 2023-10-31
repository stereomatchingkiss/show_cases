#pragma once

#include "object_detect_model_enum.hpp"

struct object_detect_model_select_config
{
    float confidence_ = 0.25f;
    int process_size_ = 416;
    float nms_ = 0.3f;
    object_detect_model_enum model_ = object_detect_model_enum::nanodet_plus_m_320;
};
