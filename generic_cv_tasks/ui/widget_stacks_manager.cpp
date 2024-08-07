#include "widget_stacks_manager.hpp"
#include "ui_widget_stacks_manager.h"

#ifdef ACTION_CLASSIFY_IS_ON
#include "action_classify/widget_stacks_action_classify.hpp"
#endif

#ifdef FALL_DOWN_DET_IS_ON
#include "fall_down_detect/widget_stacks_fall_down_detection.hpp"
#endif

#ifdef OBJ_DET_IS_ON
#include "obj_detect/widget_stacks_object_tracking.hpp"
#endif

#include "widget_tasks_selection.hpp"

#include "../config/config_tasks_selection.hpp"

#include "../global/global_object.hpp"

#include <utils/unique_name_generator.hpp>

#include <QDebug>

#include <QJsonObject>
#include <QMessageBox>

namespace{

inline QString state_cam_name(){ return "state_cam_name"; }
inline QString state_stacks_action_classify(){ return "state_stacks_action_classify"; }
inline QString state_stacks_fall_down_detection(){ return "state_stacks_fall_down_detection"; }
inline QString state_stacks_object_tracking(){ return "state_stacks_object_tracking"; }
inline QString state_tasks_selection(){ return "state_tasks_selection"; }

inline QString state_version(){ return "state_version"; }

}

widget_stacks_manager::widget_stacks_manager(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::widget_stacks_manager),
    msg_box_{new QMessageBox{this}},
    widget_stacks_{nullptr},
    widget_tasks_selection_{new widget_tasks_selection}
{
    ui->setupUi(this);

    ui->labelInfo->setVisible(false);
    init_stacked_widget();
}

widget_stacks_manager::~widget_stacks_manager()
{
    delete ui;
}

QJsonObject widget_stacks_manager::get_states()
{
    QJsonObject obj;

    if(widget_stacks_){
        switch(widget_tasks_selection_->get_config().task_){
        case enum_config_tasks::action_classify:{
#ifdef ACTION_CLASSIFY_IS_ON
            auto widget = static_cast<widget_stacks_action_classify*>(widget_stacks_);
            obj[state_stacks_action_classify()] = widget->get_states();
            obj[state_stacks_fall_down_detection()] = stacks_states_[state_stacks_fall_down_detection()].toObject();
            obj[state_stacks_object_tracking()] = stacks_states_[state_stacks_object_tracking()].toObject();
#endif
            break;
        }
        case enum_config_tasks::fall_down_detection:{
#ifdef FALL_DOWN_DET_IS_ON
            auto widget = static_cast<widget_stacks_fall_down_detection*>(widget_stacks_);
            obj[state_stacks_fall_down_detection()] = widget->get_states();
            obj[state_stacks_action_classify()] = stacks_states_[state_stacks_action_classify()].toObject();
            obj[state_stacks_object_tracking()] = stacks_states_[state_stacks_object_tracking()].toObject();
#endif
            break;
        }
        case enum_config_tasks::object_tracking:{
#ifdef OBJ_DET_IS_ON
            auto widget = static_cast<widget_stacks_object_tracking*>(widget_stacks_);
            obj[state_stacks_object_tracking()] = widget->get_states();
            obj[state_stacks_action_classify()] = stacks_states_[state_stacks_action_classify()].toObject();
            obj[state_stacks_fall_down_detection()] = stacks_states_[state_stacks_fall_down_detection()].toObject();
#endif
            break;
        }
        default:
            break;
        }

        stacks_states_ = obj;
    }

    obj[state_cam_name()] = ui->labelInfo->text();
    obj[state_tasks_selection()] = widget_tasks_selection_->get_states();
    obj[state_version()] = "1.0";

    return obj;
}

void widget_stacks_manager::set_stream_name(QString const &text)
{
    ui->labelInfo->setText(text);
    widget_tasks_selection_->set_stream_name(text);
    get_unique_name_generator().add_unique_name(widget_tasks_selection_->get_stream_name().toStdString(), this);
}

void widget_stacks_manager::set_states(const QJsonObject &val)
{
    stacks_states_ = val;
    if(stacks_states_.contains(state_tasks_selection())){        
        widget_tasks_selection_->set_states(stacks_states_[state_tasks_selection()].toObject());
    }

    if(val.contains(state_cam_name())){
        ui->labelInfo->setText(val[state_cam_name()].toString());
    }    

    setup_stacks();
}

void widget_stacks_manager::on_pushButtonNext_clicked()
{    
    if(ui->stackedWidget->currentWidget() == widget_tasks_selection_){
        if(get_unique_name_generator().add_unique_name(widget_tasks_selection_->get_stream_name().toStdString(), this)){
            ui->labelInfo->setVisible(true);
            ui->labelInfo->setText(widget_tasks_selection_->get_stream_name());
            ui->pushButtonNext->setVisible(false);            
            setup_stacks();            
            ui->stackedWidget->setCurrentWidget(widget_stacks_);
        }else{
            msg_box_->warning(this, tr("Warning"), tr("Stream name must be unique"));
        }
    }else{
        ui->labelInfo->setVisible(false);
    }
}

void widget_stacks_manager::switch_to_task_selection_page()
{
    if(more_than_one_task()){
        ui->pushButtonNext->setVisible(true);
        ui->stackedWidget->setCurrentWidget(widget_tasks_selection_);
    }else{
        ui->pushButtonNext->setVisible(false);
    }

    ui->labelInfo->setVisible(false);

    stacks_states_ = get_states();
}

QString widget_stacks_manager::get_cam_name() const
{
    return ui->labelInfo->text();
}

void widget_stacks_manager::init_stacked_widget()
{
    if(more_than_one_task()){
        ui->pushButtonNext->setVisible(true);
        ui->stackedWidget->addWidget(widget_tasks_selection_);
        ui->stackedWidget->setCurrentWidget(widget_tasks_selection_);
    }else{
        ui->pushButtonNext->setVisible(false);
        setup_stacks();
    }

    connect(widget_tasks_selection_, &widget_tasks_selection::task_change, this, &widget_stacks_manager::task_change);
}

void widget_stacks_manager::setup_stacks()
{
    switch(widget_tasks_selection_->get_config().task_){
    case enum_config_tasks::action_classify:{
#ifdef ACTION_CLASSIFY_IS_ON
        setup_stack_widget(new widget_stacks_action_classify, state_stacks_action_classify());
#endif
        break;
    }
    case enum_config_tasks::object_tracking:{
#ifdef OBJ_DET_IS_ON
        setup_stack_widget(new widget_stacks_object_tracking, state_stacks_object_tracking());
#endif
        break;
    }
    case enum_config_tasks::fall_down_detection:{
#ifdef FALL_DOWN_DET_IS_ON
        setup_stack_widget(new widget_stacks_fall_down_detection, state_stacks_fall_down_detection());
#endif
        break;
    }
    default:{        
        break;
    }
    }
}

void widget_stacks_manager::task_change(int)
{
    setup_stacks();
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
