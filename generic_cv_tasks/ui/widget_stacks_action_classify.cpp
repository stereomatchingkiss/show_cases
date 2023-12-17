#include "widget_stacks_action_classify.hpp"
#include "ui_widget_stacks_action_classify.h"

#include "widget_action_classify_model_select.hpp"
#include "widget_select_action_to_classify.hpp"
#include "widget_source_selection.hpp"

#include <QJsonObject>

namespace{

QString const state_widget_action_classify_model_select("state_widget_action_classify_model_select");
QString const state_widget_select_action_to_classify("state_widget_select_action_to_classify");
QString const state_widget_source_selection("state_widget_source_selection");

}

widget_stacks_action_classify::widget_stacks_action_classify(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::widget_stacks_action_classify)
{
    ui->setupUi(this);

    init_stacked_widget();
}

widget_stacks_action_classify::~widget_stacks_action_classify()
{
    delete ui;
}

QJsonObject widget_stacks_action_classify::get_states() const
{
    QJsonObject obj;
    obj[state_widget_action_classify_model_select] = widget_action_classify_model_select_->get_states();
    obj[state_widget_select_action_to_classify] = widget_select_action_to_classify_->get_states();
    obj[state_widget_source_selection] = widget_source_selection_->get_states();

    return obj;
}

void widget_stacks_action_classify::set_states(const QJsonObject &val)
{
    if(val.contains(state_widget_action_classify_model_select)){
        widget_action_classify_model_select_->set_states(val[state_widget_action_classify_model_select].toObject());
    }
    if(val.contains(state_widget_select_action_to_classify)){
        widget_select_action_to_classify_->set_states(val[state_widget_select_action_to_classify].toObject());
    }
    if(val.contains(state_widget_source_selection)){
        widget_source_selection_->set_states(val[state_widget_source_selection].toObject());
    }
}

void widget_stacks_action_classify::init_stacked_widget()
{
    widget_action_classify_model_select_ = new widget_action_classify_model_select;
    widget_select_action_to_classify_ = new widget_select_action_to_classify;
    widget_source_selection_ = new widget_source_selection;

    ui->stackedWidget->addWidget(widget_action_classify_model_select_);
    ui->stackedWidget->addWidget(widget_select_action_to_classify_);
    ui->stackedWidget->addWidget(widget_source_selection_);
}

void widget_stacks_action_classify::on_pushButtonPrev_clicked()
{
    if(ui->stackedWidget->currentWidget() == widget_select_action_to_classify_){
        ui->stackedWidget->setCurrentWidget(widget_action_classify_model_select_);
    }else if(ui->stackedWidget->currentWidget() == widget_source_selection_){
        ui->stackedWidget->setCurrentWidget(widget_select_action_to_classify_);
    }else if(ui->stackedWidget->currentWidget() == widget_select_action_to_classify_){
        emit enable_next_button();
    }
}


void widget_stacks_action_classify::on_pushButtonNext_clicked()
{
    if(ui->stackedWidget->currentWidget() == widget_action_classify_model_select_){
        ui->stackedWidget->setCurrentWidget(widget_select_action_to_classify_);
    }else if(ui->stackedWidget->currentWidget() == widget_select_action_to_classify_){
        ui->stackedWidget->setCurrentWidget(widget_source_selection_);
    }
}

