#include "widget_stacks_manager.hpp"
#include "ui_widget_stacks_manager.h"

#include "action_classify/widget_stacks_action_classify.hpp"
#include "obj_detect/widget_stacks_object_tracking.hpp"
#include "widget_tasks_selection.hpp"

#include "../config/config_tasks_selection.hpp"

#include <QDebug>

#include <QJsonObject>

using namespace flt;
using namespace flt::mm;

namespace{

QString const state_stacks_action_classify("state_stacks_action_classify");
QString const state_stacks_object_tracking("state_stacks_object_tracking");
QString const state_tasks_selection("state_tasks_selection");

}

widget_stacks_manager::widget_stacks_manager(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::widget_stacks_manager),
    widget_stacks_{nullptr},
    widget_tasks_selection_{new widget_tasks_selection}
{
    ui->setupUi(this);

    init_stacked_widget();

    setMinimumSize(QSize(600, 400));
}

widget_stacks_manager::~widget_stacks_manager()
{
    delete ui;
}

QJsonObject widget_stacks_manager::get_states()
{
    QJsonObject obj;

    switch(widget_tasks_selection_->get_config().task_){
    case enum_config_tasks::action_classify:{
        auto widget = static_cast<widget_stacks_action_classify*>(widget_stacks_);
        obj[state_stacks_action_classify] = widget->get_states();
        obj[state_stacks_object_tracking] = stacks_states_[state_stacks_object_tracking].toObject();

        break;
    }
    case enum_config_tasks::object_tracking:{
        auto widget = static_cast<widget_stacks_object_tracking*>(widget_stacks_);
        obj[state_stacks_action_classify] = stacks_states_[state_stacks_action_classify].toObject();
        obj[state_stacks_object_tracking] = widget->get_states();

        break;
    }
    default:
        break;
    }
    obj[state_tasks_selection] = widget_tasks_selection_->get_states();

    stacks_states_ = obj;

    return obj;
}

void widget_stacks_manager::set_states(const QJsonObject &val)
{
    stacks_states_ = val;
    if(stacks_states_.contains(state_tasks_selection)){
        widget_tasks_selection_->set_states(stacks_states_[state_tasks_selection].toObject());
    }

    setup_stacks();
}

void widget_stacks_manager::on_pushButtonNext_clicked()
{    
    if(ui->stackedWidget->currentWidget() == widget_tasks_selection_){
        ui->pushButtonNext->setVisible(false);
        setup_stacks();
        ui->stackedWidget->setCurrentWidget(widget_stacks_);
    }
}

void widget_stacks_manager::switch_to_task_selection_page()
{
    ui->pushButtonNext->setVisible(true);
    ui->stackedWidget->setCurrentWidget(widget_tasks_selection_);
}

void widget_stacks_manager::init_stacked_widget()
{    
    ui->stackedWidget->addWidget(widget_tasks_selection_);
    ui->stackedWidget->setCurrentWidget(widget_tasks_selection_);
}

void widget_stacks_manager::setup_stacks()
{
    switch(widget_tasks_selection_->get_config().task_){
    case enum_config_tasks::action_classify:{
        setup_stack_widget(new widget_stacks_action_classify, state_stacks_action_classify);
        break;
    }
    case enum_config_tasks::object_tracking:{        
        setup_stack_widget(new widget_stacks_object_tracking, state_stacks_object_tracking);
        break;
    }
    default:{
        auto widget = new widget_stacks_object_tracking;
        update_stack_widget(widget);
        break;
    }
    }
}

void widget_stacks_manager::update_stack_widget(QWidget *widget)
{
    if(widget_stacks_ == nullptr){
        widget_stacks_ = widget;
        ui->stackedWidget->addWidget(widget_stacks_);
    }else{
        ui->stackedWidget->removeWidget(widget_stacks_);
        widget_stacks_->deleteLater();
        widget_stacks_ = widget;
        ui->stackedWidget->addWidget(widget_stacks_);
    }
}
