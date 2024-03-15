#include "widget_pose_estimation_params.hpp"
#include "ui_widget_pose_estimation_params.h"

#include "../config/config_pose_estimation_worker.hpp"

widget_pose_estimation_params::widget_pose_estimation_params(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::widget_pose_estimation_params)
{
    ui->setupUi(this);

    ui->spinBoxConfidence->setValue(30);
}

widget_pose_estimation_params::~widget_pose_estimation_params()
{
    delete ui;
}

config_psoe_estimation_worker widget_pose_estimation_params::get_config() const
{
    config_psoe_estimation_worker config;
    config.confidence_ = ui->spinBoxConfidence->value() / 100.0f;

    return config;
}
