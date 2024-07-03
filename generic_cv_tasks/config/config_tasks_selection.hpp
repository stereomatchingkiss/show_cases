#pragma once

enum class enum_config_tasks
{
    object_tracking,
    action_classify,
    fall_down_detection
};

struct config_tasks_selection
{
    enum_config_tasks task_;
};
