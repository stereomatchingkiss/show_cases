#include "widget_stacks_action_classify.hpp"
#include "ui_widget_stacks_action_classify.h"

#include "widget_action_classify_model_select.hpp"
#include "widget_select_action_to_classify.hpp"

#include <QJsonObject>

namespace{



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

    return obj;
}

void widget_stacks_action_classify::set_states(const QJsonObject &val)
{

}

void widget_stacks_action_classify::init_stacked_widget()
{
    widget_action_classify_model_select_ = new widget_action_classify_model_select;
    widget_select_action_to_classify_ = new widget_select_action_to_classify;

    ui->stackedWidget->addWidget(widget_action_classify_model_select_);
    ui->stackedWidget->addWidget(widget_select_action_to_classify_);
}

void widget_stacks_action_classify::on_pushButtonPrev_clicked()
{
    if(ui->stackedWidget->currentWidget() == widget_select_action_to_classify_){
        ui->stackedWidget->setCurrentWidget(widget_action_classify_model_select_);
    }else if(ui->stackedWidget->currentWidget() == widget_select_action_to_classify_){
        emit enable_next_button();
    }
}


void widget_stacks_action_classify::on_pushButtonNext_clicked()
{
    if(ui->stackedWidget->currentWidget() == widget_action_classify_model_select_){
        ui->stackedWidget->setCurrentWidget(widget_select_action_to_classify_);
    }
}

