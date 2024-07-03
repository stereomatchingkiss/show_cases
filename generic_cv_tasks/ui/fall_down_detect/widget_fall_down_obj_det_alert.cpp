#include "widget_fall_down_obj_det_alert.hpp"
#include "ui_widget_fall_down_obj_det_alert.h"

widget_fall_down_obj_det_alert::widget_fall_down_obj_det_alert(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::widget_fall_down_obj_det_alert)
{
    ui->setupUi(this);
}

widget_fall_down_obj_det_alert::~widget_fall_down_obj_det_alert()
{
    delete ui;
}
