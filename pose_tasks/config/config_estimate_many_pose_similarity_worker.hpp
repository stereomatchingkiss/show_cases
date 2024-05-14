#pragma once

#include "pose_estimation_model_enum.hpp"

#include <QString>

struct config_estimate_many_pose_similarity_worker
{
    float confidence_ = 0.3f;    
    pose_estimation_model_enum model_ = pose_estimation_model_enum::movenet_single_thunder;

    QString data_folder_;
    QString image_folder_;

    QString generate_json_info() const;
};
