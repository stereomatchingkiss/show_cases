#pragma once

#include <QUrl>

struct config_alert_sender
{
    bool activate_ = false;
    bool send_by_text_ = true;
    QUrl url_;    
};
