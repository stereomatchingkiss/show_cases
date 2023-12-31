#pragma once

#include "config_action_classify_alert.hpp"
#include "config_action_classify_model_select.hpp"
#include "config_alert_sender.hpp"
#include "config_select_action_to_classify.hpp"

#include <QRectF>

struct config_ppstm_v2_worker
{
    config_action_classify_alert config_action_classify_alert_;
    config_action_classify_model_select config_action_classify_model_select_;
    config_alert_sender config_alert_sender_;
    config_select_action_to_classify config_select_action_to_classify_;

    QRectF roi_;
};
