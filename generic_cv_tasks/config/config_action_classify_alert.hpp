#pragma once

struct config_action_classify_alert
{
    int alert_if_action_detected_duration_sec_ = 5;
    bool alert_if_action_detected_on_ = true;
};
