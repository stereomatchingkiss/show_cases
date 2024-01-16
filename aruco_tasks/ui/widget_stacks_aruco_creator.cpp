#include "widget_stacks_aruco_creator.hpp"
#include "ui_widget_stacks_aruco_creator.h"

#include "widget_aruco_creator.hpp"

widget_stacks_aruco_creator::widget_stacks_aruco_creator(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::widget_stacks_aruco_creator)
{
    ui->setupUi(this);

    init_stacked_widget();
}

widget_stacks_aruco_creator::~widget_stacks_aruco_creator()
{
    delete ui;
}

void widget_stacks_aruco_creator::init_stacked_widget()
{
    widget_aruco_creator_ = new widget_aruco_creator;

    ui->stackedWidget->addWidget(widget_aruco_creator_);

    ui->stackedWidget->setCurrentWidget(widget_aruco_creator_);
}

void widget_stacks_aruco_creator::on_pushButton_clicked()
{
    emit prev_button_clicked();
}

