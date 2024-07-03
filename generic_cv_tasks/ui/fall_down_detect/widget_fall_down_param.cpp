#include "widget_fall_down_param.hpp"
#include "ui_widget_fall_down_param.h"

#include <QJsonObject>

namespace{

inline QString state_hw_ratio(){ return "state_hw_ratio"; }

inline QString state_version(){ return "state_version"; }

}

widget_fall_down_param::widget_fall_down_param(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::widget_fall_down_param)
{
    ui->setupUi(this);
}

widget_fall_down_param::~widget_fall_down_param()
{
    delete ui;
}

QJsonObject widget_fall_down_param::get_states() const
{
    QJsonObject obj;
    obj[state_hw_ratio()] = ui->doubleSpinBoxWidthHeightRatio->value();
    obj[state_version()] = "1.0";

    return obj;
}

void widget_fall_down_param::set_states(const QJsonObject &val)
{
    if(val.contains(state_hw_ratio())){
        ui->doubleSpinBoxWidthHeightRatio->setValue(val[state_hw_ratio()].toDouble());
    }
}
