#include "widget_tasks_selection.hpp"
#include "ui_widget_tasks_selection.h"

#include "../config/config_tasks_selection.hpp"

#include <QJsonObject>

namespace{

QString const state_mode("state_mode");

}

widget_tasks_selection::widget_tasks_selection(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::widget_tasks_selection)
{
    ui->setupUi(this);

    ui->comboBoxTasks->addItem(tr("Pose estimation"));
}

widget_tasks_selection::~widget_tasks_selection()
{
    delete ui;
}

config_tasks_selection widget_tasks_selection::get_config() const noexcept
{
    config_tasks_selection resutls;
    switch(ui->comboBoxTasks->currentIndex()){
    case 0:{
        resutls.task_ = enum_config_tasks::pose_estimation;
        break;
    }
    default:
        resutls.task_ = enum_config_tasks::pose_estimation;
        break;
    }

    return resutls;
}

QJsonObject widget_tasks_selection::get_states() const
{
    QJsonObject obj;
    obj[state_mode] = static_cast<int>(get_config().task_);

    return obj;
}

void widget_tasks_selection::set_states(const QJsonObject &val)
{
    if(val.contains(state_mode)){
        ui->comboBoxTasks->setCurrentIndex(val[state_mode].toInt());
    }
}

void widget_tasks_selection::on_comboBoxTasks_activated(int index)
{
    emit task_selected(index);
}

