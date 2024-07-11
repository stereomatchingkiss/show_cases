#pragma once

struct config_fall_down_condition
{
    int number_of_consecutive_falls_ = 3;
    bool show_width_height_ratio_ = false;
    float width_height_ratio_ = 1.5f;
};
