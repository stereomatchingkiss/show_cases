#pragma once

#include "config_alert_sender.hpp"
#include "config_object_detect_model_select.hpp"
#include "config_select_object_to_detect.hpp"
#include "config_tracker_alert.hpp"

#include <multimedia/stream_enum.hpp>

#include <QRectF>

struct config_nanodet_worker{
    config_alert_sender config_alert_sender_;
    config_object_detect_model_select config_object_detect_model_select_;
    config_select_object_to_detect config_select_object_to_detect_;
    config_tracker_alert config_tracker_alert_;

    QRectF roi_;

    flt::mm::stream_source_type source_type_;
};
