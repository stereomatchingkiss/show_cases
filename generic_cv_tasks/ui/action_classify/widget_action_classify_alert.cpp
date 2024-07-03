#include "widget_action_classify_alert.hpp"
#include "ui_widget_action_classify_alert.h"

#include "../../config/config_action_classify_alert.hpp"

#include <QJsonObject>

namespace{

QString const state_alert_if_action_detected_duration_sec("state_alert_if_action_detected_duration_sec");
QString const state_alert_if_action_detected_on("state_alert_if_action_detected_on");

}

widget_action_classify_alert::widget_action_classify_alert(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::widget_action_classify_alert)
{
    ui->setupUi(this);
}

widget_action_classify_alert::~widget_action_classify_alert()
{
    delete ui;
}

config_action_classify_alert widget_action_classify_alert::get_config() const
{
    config_action_classify_alert config;
    config.alert_if_action_detected_duration_sec_ = ui->spinBoxAlertIfActionDetectDuration->value();
    config.alert_if_action_detected_on_ = ui->groupBoxAlertIfActionDetected->isChecked();

    return config;
}

QJsonObject widget_action_classify_alert::get_states() const
{
    QJsonObject obj;
    obj[state_alert_if_action_detected_duration_sec] = ui->spinBoxAlertIfActionDetectDuration->value();
    obj[state_alert_if_action_detected_on] = ui->groupBoxAlertIfActionDetected->isChecked();

    return obj;
}

void widget_action_classify_alert::set_states(QJsonObject const &val)
{
    if(val.contains(state_alert_if_action_detected_duration_sec)){
        ui->spinBoxAlertIfActionDetectDuration->setValue(val[state_alert_if_action_detected_duration_sec].toInt());
    }
    if(val.contains(state_alert_if_action_detected_on)){
        ui->groupBoxAlertIfActionDetected->setChecked(val[state_alert_if_action_detected_on].toBool());
    }
}
