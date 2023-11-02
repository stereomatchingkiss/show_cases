#include "widget_tracker_alert.hpp"
#include "ui_widget_tracker_alert.h"

#include "../config/config_tracker_alert.hpp"

#include <QJsonObject>

namespace{

QString const state_alert_if_stay_in_roi_duration("state_alert_if_stay_in_roi_duration");
QString const state_alert_if_stay_in_roi_on("state_alert_if_stay_in_roi_on");
QString const state_save_reports("state_save_reports");

}

widget_tracker_alert::widget_tracker_alert(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::widget_tracker_alert)
{
    ui->setupUi(this);    
}

widget_tracker_alert::~widget_tracker_alert()
{
    delete ui;
}

config_tracker_alert widget_tracker_alert::get_config() const noexcept
{
    config_tracker_alert config;
    config.alert_if_stay_in_roi_duration_sec_ = ui->spinBoxAlertIfStayInRoiDuration->value();
    config.alert_if_stay_in_roi_on_ = ui->groupBoxAlertIfStayInRoiDuration->isChecked();
    config.save_checks_ = ui->checkBoxSaveReports->isChecked();

    return config;
}

QJsonObject widget_tracker_alert::get_states() const
{
    QJsonObject obj;
    obj[state_alert_if_stay_in_roi_duration] = ui->spinBoxAlertIfStayInRoiDuration->value();
    obj[state_alert_if_stay_in_roi_on] = ui->groupBoxAlertIfStayInRoiDuration->isChecked();
    obj[state_save_reports] = ui->checkBoxSaveReports->isChecked();

    return obj;
}

void widget_tracker_alert::set_states(QJsonObject const &val)
{
    if(val.contains(state_alert_if_stay_in_roi_duration)){
        ui->spinBoxAlertIfStayInRoiDuration->setValue(val[state_alert_if_stay_in_roi_duration].toInt());
    }
    if(val.contains(state_alert_if_stay_in_roi_on)){
        ui->groupBoxAlertIfStayInRoiDuration->setChecked(val[state_alert_if_stay_in_roi_on].toBool());
    }
    if(val.contains(state_save_reports)){
        ui->checkBoxSaveReports->setChecked(val[state_save_reports].toBool());
    }
}
