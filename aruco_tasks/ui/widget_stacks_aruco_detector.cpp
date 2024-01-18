#include "widget_stacks_aruco_detector.hpp"
#include "ui_widget_stacks_aruco_detector.h"

#include "dialog_aruco_detector_params.hpp"

widget_stacks_aruco_detector::widget_stacks_aruco_detector(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::widget_stacks_aruco_detector)
{
    ui->setupUi(this);

    init_stacked_widget();
}

widget_stacks_aruco_detector::~widget_stacks_aruco_detector()
{
    delete ui;
}

void widget_stacks_aruco_detector::init_stacked_widget()
{
    dialog_aruco_detector_params_ = new dialog_aruco_detector_params;

    ui->stackedWidget->addWidget(dialog_aruco_detector_params_);

    ui->stackedWidget->setCurrentWidget(dialog_aruco_detector_params_);
}

void widget_stacks_aruco_detector::on_pushButtonPrev_clicked()
{
    emit prev_button_clicked();
}

