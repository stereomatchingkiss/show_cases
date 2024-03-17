#pragma once

enum class enum_config_tasks
{
    pose_estimation,
    estimate_pose_similarity
};

struct config_tasks_selection
{
    enum_config_tasks task_;
};
