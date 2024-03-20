#ifndef CONFIG_ESTIMATE_MANY_POSE_SIMILARITY_WORKER_HPP
#define CONFIG_ESTIMATE_MANY_POSE_SIMILARITY_WORKER_HPP

#include "pose_estimation_model_enum.hpp"

#include <QString>

struct config_estimate_many_pose_similarity_worker
{
    float confidence_ = 0.3f;
    pose_estimation_model_enum model_ = pose_estimation_model_enum::movenet_single_thunder;

    QString data_folder_;
    QString image_folder_;
};

#endif // CONFIG_ESTIMATE_MANY_POSE_SIMILARITY_WORKER_HPP
