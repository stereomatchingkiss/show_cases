#pragma once

#include "config_object_detect_model_select.hpp"
#include "config_select_object_to_detect.hpp"

#include <QRectF>

struct config_generic_obj_detector
{
    config_object_detect_model_select config_object_detect_model_select_;
    config_select_object_to_detect config_select_object_to_detect_;

    QRectF roi_;
};
