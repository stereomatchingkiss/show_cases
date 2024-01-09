#pragma once

enum class object_detect_model_enum
{
    nanodet_plus_m_320,
    nanodet_plus_m_416,
};

struct config_object_detect_model_select
{
    float confidence_ = 0.4f;
    int process_size_ = 416;
    float nms_ = 0.3f;
    object_detect_model_enum model_ = object_detect_model_enum::nanodet_plus_m_416;
};
