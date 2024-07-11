#include "widget_fall_down_obj_det_alert.hpp"
#include "ui_widget_fall_down_obj_det_alert.h"

#include "../../config/config_fall_down_obj_det_alert.hpp"

#include "../../global/global_keywords.hpp"

#include <QJsonObject>

namespace{

inline QString state_warning_condition(){ return "state_warning_condition"; }
inline QString state_warning_periodic(){ return "state_warning_periodic"; }

}

widget_fall_down_obj_det_alert::widget_fall_down_obj_det_alert(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::widget_fall_down_obj_det_alert)
{
    ui->setupUi(this);

    ui->radioButtonWarningOnce->setChecked(true);
}

widget_fall_down_obj_det_alert::~widget_fall_down_obj_det_alert()
{
    delete ui;
}

config_fall_down_obj_det_alert widget_fall_down_obj_det_alert::get_config() const
{
    config_fall_down_obj_det_alert config;

    return config;
}

QJsonObject widget_fall_down_obj_det_alert::get_states() const
{
    global_keywords const gk;

    QJsonObject obj;
    obj[state_warning_condition()] = get_warning_condition();
    obj[state_warning_periodic()] = ui->spinBoxWarningPeriodic->value();

    obj[gk.state_version()] = "1.0";

    return obj;
}

void widget_fall_down_obj_det_alert::set_states(const QJsonObject &val)
{
    if(val.contains(state_warning_condition())){
        using wt = fall_down_warning_type;

        switch(static_cast<wt>(val[state_warning_condition()].toInt())){
        case wt::issue_a_warning:{
            ui->radioButtonWarningOnce->setChecked(true);
            break;
        }
        case wt::issue_warning_periodic:{
            ui->radioButtonWarningPeriodic->setChecked(true);
            break;
        }
        }
    }

    if(val.contains(state_warning_periodic())){
        ui->spinBoxWarningPeriodic->setValue(val[state_warning_periodic()].toInt());
    }
}

int widget_fall_down_obj_det_alert::get_warning_condition() const
{
    using wt = fall_down_warning_type;

    if(ui->radioButtonWarningPeriodic->isChecked()){
        return static_cast<int>(wt::issue_warning_periodic);
    }

    return static_cast<int>(wt::issue_a_warning);
}
