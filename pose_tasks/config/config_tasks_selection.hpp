#ifndef CONFIG_TASKS_SELECTION_HPP
#define CONFIG_TASKS_SELECTION_HPP

enum class enum_config_tasks
{
    pose_estimation
};

struct config_tasks_selection
{
    enum_config_tasks task_;
};

#endif // CONFIG_TASKS_SELECTION_HPP
