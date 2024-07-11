#pragma once

#include "config_alert_sender.hpp"
#include "config_fall_down_obj_det_alert.hpp"
#include "config_fall_down_condition.hpp"
#include "config_object_detect_model_select.hpp"

#include <QRectF>

struct config_fall_down_obj_det_worker
{
    config_alert_sender config_alert_sender_;
    config_fall_down_condition config_fall_down_condition_;
    config_fall_down_obj_det_alert config_fall_down_obj_det_alert_;
    config_object_detect_model_select config_object_detect_model_select_;

    QRectF roi_;
};
