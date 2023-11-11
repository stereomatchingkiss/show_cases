#pragma once

#include <QUrl>

struct config_alert_sender
{
    bool activate_ = false;
    QUrl url_;
};
