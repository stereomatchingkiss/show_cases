#pragma once

#include <multimedia/stream_enum.hpp>

#include "pose_estimation_model_enum.hpp"

struct config_psoe_estimation_worker
{
    float confidence_ = 0.3f;
    pose_estimation_model_enum model_ = pose_estimation_model_enum::movenet_single_thunder;
    flt::mm::stream_source_type source_type_;
};
