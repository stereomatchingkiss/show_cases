#include "widget_estimate_many_pose_similarity_params.hpp"
#include "ui_widget_estimate_many_pose_similarity_params.h"

#include "../config/config_estimate_many_pose_similarity_worker.hpp"

#include <QFileDialog>

widget_estimate_many_pose_similarity_params::widget_estimate_many_pose_similarity_params(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::widget_estimate_many_pose_similarity_params)
{
    ui->setupUi(this);
}

widget_estimate_many_pose_similarity_params::~widget_estimate_many_pose_similarity_params()
{
    delete ui;
}

config_estimate_many_pose_similarity_worker widget_estimate_many_pose_similarity_params::get_config() const
{
    config_estimate_many_pose_similarity_worker config;
    config.confidence_ = ui->spinBoxConfidence->value() / 100.0f;
    config.data_folder_ = ui->lineEditData->text();
    config.image_folder_ = ui->lineEditImages->text();

    return config;
}

void widget_estimate_many_pose_similarity_params::on_pushButtonImages_clicked()
{
    auto const url = QFileDialog::getExistingDirectory(this, tr("Select directory you want to search the most similar pose"));
    if(!url.isEmpty()){
        ui->lineEditImages->setText(url);
    }
}

void widget_estimate_many_pose_similarity_params::on_pushButtonData_clicked()
{
    auto const url = QFileDialog::getExistingDirectory(this, tr("Select directory contains the pose information(they save as json)"));
    if(!url.isEmpty()){
        ui->lineEditData->setText(url);
    }
}

