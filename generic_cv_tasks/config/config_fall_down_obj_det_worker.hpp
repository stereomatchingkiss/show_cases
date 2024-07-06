#pragma once

#include "config_alert_sender.hpp"
#include "config_object_detect_model_select.hpp"
#include "config_select_object_to_detect.hpp"
#include "config_tracker_alert.hpp"

#include <QRectF>

struct config_fall_down_obj_det
{
    config_alert_sender config_alert_sender_;
    config_object_detect_model_select config_object_detect_model_select_;
    config_select_object_to_detect config_select_object_to_detect_;
    config_tracker_alert config_tracker_alert_;

    QRectF roi_;
    float width_height_ratio_ = 0.4f;
};
