#include "widget_stacks_anpr.hpp"
#include "ui_widget_stacks_anpr.h"

#include "widget_object_detect_model_select.hpp"
#include "widget_source_selection.hpp"

widget_stacks_anpr::widget_stacks_anpr(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::widget_stacks_anpr)
{
    ui->setupUi(this);

    init_stacked_widget();
}

widget_stacks_anpr::~widget_stacks_anpr()
{
    delete ui;
}

void widget_stacks_anpr::on_pushButtonPrev_clicked()
{

}

void widget_stacks_anpr::on_pushButtonNext_clicked()
{

}

void widget_stacks_anpr::init_stacked_widget()
{
    widget_object_detect_model_select_ = new widget_object_detect_model_select;
    widget_source_selection_ = new widget_source_selection;

    ui->stackedWidget->addWidget(widget_object_detect_model_select_);
    ui->stackedWidget->addWidget(widget_source_selection_);

    ui->stackedWidget->setCurrentWidget(widget_object_detect_model_select_);
}

