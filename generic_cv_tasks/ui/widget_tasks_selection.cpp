#include "widget_tasks_selection.hpp"
#include "ui_widget_tasks_selection.h"

#include "../config/config_tasks_selection.hpp"

#include <QJsonObject>

namespace{

inline QString state_mode(){ return "state_mode"; }

inline QString state_version(){ return "state_version"; }

}

widget_tasks_selection::widget_tasks_selection(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::widget_tasks_selection)
{
    ui->setupUi(this);

    ui->comboBoxTasks->addItem(tr("Object tracking"));
    ui->comboBoxTasks->addItem(tr("Action classify"));
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
        resutls.task_ = enum_config_tasks::object_tracking;
        break;
    }
    case 1:{
        resutls.task_ = enum_config_tasks::action_classify;
        break;
    }
    default:
        break;
    }

    return resutls;
}

QJsonObject widget_tasks_selection::get_states() const
{
    QJsonObject obj;
    obj[state_mode()] = ui->comboBoxTasks->currentText();

    obj[state_version()] = "1.0";

    return obj;
}

void widget_tasks_selection::set_states(const QJsonObject &val)
{
    if(val.contains(state_mode())){
        if(val[state_mode()].toString() == "Object tracking"){
            ui->comboBoxTasks->setCurrentIndex(0);
        }else{
            ui->comboBoxTasks->setCurrentIndex(1);
        }
    }
}
