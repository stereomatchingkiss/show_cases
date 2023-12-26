#include "widget_face_recognition_model_select.hpp"
#include "ui_widget_face_recognition_model_select.h"

#include "../config/config_face_recognition_model_select.hpp"

#include <QJsonObject>

namespace{

QString const state_face_reg_confidence("state_face_reg_confidence");

}

widget_face_recognition_model_select::widget_face_recognition_model_select(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::widget_face_recognition_model_select)
{
    ui->setupUi(this);

    ui->groupBoxSelectModel->setVisible(false);
}

widget_face_recognition_model_select::~widget_face_recognition_model_select()
{
    delete ui;
}

config_face_recognition_model_select widget_face_recognition_model_select::get_config() const
{
    config_face_recognition_model_select config;
    config.confidence_ = ui->spinBoxConfidence->value() / 100.0f;

    return config;
}

QJsonObject widget_face_recognition_model_select::get_states() const
{
    QJsonObject obj;
    obj[state_face_reg_confidence] = ui->spinBoxConfidence->value();

    return obj;
}

void widget_face_recognition_model_select::set_states(const QJsonObject &val)
{
    if(val.contains(state_face_reg_confidence)){
        ui->spinBoxConfidence->setValue(val[state_face_reg_confidence].toInt());
    }
}
