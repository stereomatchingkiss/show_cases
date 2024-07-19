#include "widget_fall_down_condition.hpp"
#include "ui_widget_fall_down_condition.h"

#include "../../config/config_fall_down_condition.hpp"

#include <QJsonObject>

namespace{

inline QString state_hw_ratio(){ return "state_hw_ratio"; }
inline QString state_number_of_consecutive_falls() { return "state_number_of_consecutive_falls"; }
inline QString state_version(){ return "state_version"; }

}

widget_fall_down_condition::widget_fall_down_condition(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::widget_fall_down_condition)
{
    ui->setupUi(this);

    ui->doubleSpinBoxWidthHeightRatio->setRange(0.5, 2.0);
    ui->doubleSpinBoxWidthHeightRatio->setValue(0.7);

    ui->spinBoxOfConsecutiveFalls->setValue(10);
}

widget_fall_down_condition::~widget_fall_down_condition()
{
    delete ui;
}

config_fall_down_condition widget_fall_down_condition::get_config() const
{
    config_fall_down_condition config;
    config.number_of_consecutive_falls_ = ui->spinBoxOfConsecutiveFalls->value();
    config.show_width_height_ratio_ = false;
    config.width_height_ratio_ = static_cast<float>(ui->doubleSpinBoxWidthHeightRatio->value());

    return config;
}

QJsonObject widget_fall_down_condition::get_states() const
{
    QJsonObject obj;
    obj[state_number_of_consecutive_falls()] = ui->spinBoxOfConsecutiveFalls->value();
    obj[state_hw_ratio()] = ui->doubleSpinBoxWidthHeightRatio->value();
    obj[state_version()] = "1.0";

    return obj;
}

void widget_fall_down_condition::set_states(const QJsonObject &val)
{
    if(val.contains(state_number_of_consecutive_falls())){
        ui->spinBoxOfConsecutiveFalls->setValue(val[state_number_of_consecutive_falls()].toInt());
    }

    if(val.contains(state_hw_ratio())){
        ui->doubleSpinBoxWidthHeightRatio->setValue(val[state_hw_ratio()].toDouble());
    }
}
