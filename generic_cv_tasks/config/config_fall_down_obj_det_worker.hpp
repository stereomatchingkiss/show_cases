#pragma once

#include "config_alert_sender.hpp"
#include "config_object_detect_model_select.hpp"

#include <QRectF>

struct config_fall_down_obj_det_worker
{
    config_alert_sender config_alert_sender_;
    config_object_detect_model_select config_object_detect_model_select_;

    QRectF roi_;
    float width_height_ratio_ = 0.4f;
};
