#pragma once

enum class fall_down_warning_type
{
    issue_a_warning,
    issue_warning_periodic
};

struct config_fall_down_obj_det_alert
{
    bool warning_on_ = true;
    fall_down_warning_type warning_type_ = fall_down_warning_type::issue_a_warning;
    int warning_periodic_ = 5;
};
