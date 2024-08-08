#pragma once

#include "config_dialog_email_list.hpp"

#include <QStringList>
#include <QUrl>

struct config_alert_sender
{
    bool activate_ = false;
    bool save_reports_ = true;
    bool send_by_text_ = true;
    QUrl url_;

    bool email_alert_on_ = false;
    QString email_address_;
    QString email_password_;
    config_dialog_email_list config_dialog_email_list_;
};
