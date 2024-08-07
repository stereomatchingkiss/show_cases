#include "widget_tasks_selection.hpp"
#include "ui_widget_tasks_selection.h"

#include "../config/config_tasks_selection.hpp"

#include <QAbstractItemView>
#include <QListView>

#include <QJsonObject>

namespace{

inline QString state_mode(){ return "state_mode"; }
inline QString state_stream_name(){ return "state_stream_name"; }

inline QString state_version(){ return "state_version"; }

}

widget_tasks_selection::widget_tasks_selection(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::widget_tasks_selection)
{
    ui->setupUi(this);

    ui->comboBoxTasks->addItem(tr("Object tracking"));
    ui->comboBoxTasks->addItem(tr("Action classify"));
    ui->comboBoxTasks->addItem(tr("Fall down detection"));

#ifndef OBJ_DET_IS_ON
    qobject_cast<QListView *>(ui->comboBoxTasks->view())->setRowHidden(0, true);
#else
    ui->comboBoxTasks->setCurrentIndex(0);
#endif

#ifndef ACTION_CLASSIFY_IS_ON
    qobject_cast<QListView *>(ui->comboBoxTasks->view())->setRowHidden(1, true);
#else
    ui->comboBoxTasks->setCurrentIndex(1);
#endif

#ifndef FALL_DOWN_DET_IS_ON
    qobject_cast<QListView *>(ui->comboBoxTasks->view())->setRowHidden(2, true);
#else
    ui->comboBoxTasks->setCurrentIndex(2);
#endif

    connect(ui->comboBoxTasks, &QComboBox::currentIndexChanged, this, &widget_tasks_selection::task_change);
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
    case 2:{
        resutls.task_ = enum_config_tasks::fall_down_detection;
        break;
    }
    default:
        resutls.task_ = enum_config_tasks::object_tracking;
        break;
    }

    return resutls;
}

QJsonObject widget_tasks_selection::get_states() const
{
    QJsonObject obj;
    obj[state_mode()] = static_cast<int>(get_config().task_);
    obj[state_stream_name()] = ui->lineEditName->text();

    obj[state_version()] = "1.1";

    return obj;
}

void widget_tasks_selection::set_states(const QJsonObject &val)
{
    if(val.contains(state_mode())){
        if(val.contains(state_version())){
            auto const ver = val[state_version()].toString();
            if(ver == "1.0"){
                if(val[state_mode()].toString() == "Object tracking"){
                    ui->comboBoxTasks->setCurrentIndex(0);
                }else{
                    ui->comboBoxTasks->setCurrentIndex(1);
                }
            }else{
                set_task(val[state_mode()].toInt());
            }
        }else{
            set_task(val[state_mode()].toInt());
        }
    }else{
        set_task(val[state_mode()].toInt());
    }

    if(val.contains(state_stream_name())){
        ui->lineEditName->setText(val[state_stream_name()].toString());
    }
}

QString widget_tasks_selection::get_stream_name() const
{
    return ui->lineEditName->text();
}

void widget_tasks_selection::set_stream_name(const QString &text)
{
    ui->lineEditName->setText(text);
}

void widget_tasks_selection::set_task(int task)
{    
    switch(static_cast<enum_config_tasks>(task)){
    case enum_config_tasks::object_tracking:{
        ui->comboBoxTasks->setCurrentIndex(0);
        break;
    }
    case enum_config_tasks::action_classify:{
        ui->comboBoxTasks->setCurrentIndex(1);
        break;
    }
    case enum_config_tasks::fall_down_detection:{
        ui->comboBoxTasks->setCurrentIndex(2);
        break;
    }
    default:
        ui->comboBoxTasks->setCurrentIndex(0);
        break;
    }
}
