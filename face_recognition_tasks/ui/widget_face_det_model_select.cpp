#include "widget_face_det_model_select.hpp"
#include "ui_widget_face_det_model_select.h"

#include "../config/config_face_det_model_select.hpp"

#include <QJsonObject>

namespace{

QString const state_face_det_confidence("state_face_det_confidence");
QString const state_face_det_nms("state_face_det_nms");

QString const state_version("state_version");

}

widget_face_det_model_select::widget_face_det_model_select(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::widget_face_det_model_select)
{
    ui->setupUi(this);

    ui->comboBoxSelectModel->addItem("mobileFaceNetV2");

    ui->labelConfidence->setToolTip(tr("The higher the value, the more likely the model is to correctly identify the correct results.\n"
                                       "However, this can also lead to fewer of the correct results being identified."));
    ui->labelNMS->setToolTip(tr("The larger the NMS value, the fewer objects will be filtered.\n"
                                "If you find that there are many repetitive objects detected under a crowded scene, "
                                "please reduce this value."));
}

widget_face_det_model_select::~widget_face_det_model_select()
{
    delete ui;
}

config_face_det_model_select widget_face_det_model_select::get_config() const
{
    config_face_det_model_select config;
    config.confidence_ = ui->spinBoxConfidence->value() / 100.0f;
    config.nms_ = ui->spinBoxNMS->value() / 100.0f;

    return config;
}

QJsonObject widget_face_det_model_select::get_states() const
{
    QJsonObject obj;
    obj[state_face_det_confidence] = ui->spinBoxConfidence->value();
    obj[state_face_det_nms] = ui->spinBoxNMS->value();
    obj[state_version] = "1.0";

    return obj;
}

void widget_face_det_model_select::set_states(const QJsonObject &val)
{
    if(val.contains(state_face_det_confidence)){
        ui->spinBoxConfidence->setValue(val[state_face_det_confidence].toInt());
    }

    if(val.contains(state_face_det_nms)){
        ui->spinBoxNMS->setValue(val[state_face_det_nms].toInt());
    }
}
