#include "widget_object_detect_model_select.hpp"
#include "ui_widget_object_detect_model_select.h"

#include "../config/config_object_detect_model_select.hpp"

#include <QJsonObject>
#include <QLineEdit>

namespace{

QString const state_obj_det_confidence("state_obj_det_confidence");
QString const state_obj_det_model_type("state_obj_det_model_type");
QString const state_obj_det_nms("state_obj_det_nms");
QString const state_obj_det_yolov8_process_size("state_obj_det_yolo_v8_process_size");

QString const state_version("state_version");

struct model_types
{
    QString const nanodet_plus_m_320_ = "nanodet_plus_m_320";
    QString const nanodet_plus_m_416_ = "nanodet_plus_m_416";
    QString const yolo_v8_n_ = "yolo_v8_n_";

    QString get_type(int val) const noexcept
    {
        switch(val){
        case 0:
            return nanodet_plus_m_320_;
        case 1:
            return nanodet_plus_m_416_;
        case 2:
            return yolo_v8_n_;
        }

        return yolo_v8_n_;
    }

    int get_ids(QString const &val) const
    {
        if(val == nanodet_plus_m_320_){
            return 0;
        }

        if(val == nanodet_plus_m_416_){
            return 1;
        }

        if(val == yolo_v8_n_){
            return 2;
        }

        return 2;
    }
};

}

widget_object_detect_model_select::widget_object_detect_model_select(QWidget *parent) :
    QWidget(parent),
    ui(new Ui_widget_object_detect_model_select)
{
    ui->setupUi(this);

    ui->comboBoxSelectModel->addItem("nanodet_plus_m_320");
    ui->comboBoxSelectModel->addItem("nanodet_plus_m_416");
    ui->comboBoxSelectModel->addItem("yolo_v8_n");
    ui->comboBoxSelectModel->setCurrentIndex(2);

    ui->comboBoxProcessSizeYoloV8->addItem("320");
    ui->comboBoxProcessSizeYoloV8->addItem("416");
    ui->comboBoxProcessSizeYoloV8->addItem("640");
    ui->comboBoxProcessSizeYoloV8->setCurrentIndex(1);

    connect(ui->comboBoxSelectModel, &QComboBox::currentIndexChanged,
            this, &widget_object_detect_model_select::set_model_index);
}

widget_object_detect_model_select::~widget_object_detect_model_select()
{
    delete ui;
}

config_object_detect_model_select widget_object_detect_model_select::get_config() const
{
    config_object_detect_model_select results;
    results.confidence_ = static_cast<float>(ui->spinBoxConfidence->value()) / 100.0f;
    results.model_ = static_cast<object_detect_model_enum>(ui->comboBoxSelectModel->currentIndex());
    results.nms_ = static_cast<float>(ui->spinBoxNMS->value()) / 100.0f;
    switch(static_cast<object_detect_model_enum>(ui->comboBoxSelectModel->currentIndex())){
    case object_detect_model_enum::nanodet_plus_m_320:{
        results.process_size_ = 320;
        break;
    }
    case object_detect_model_enum::nanodet_plus_m_416:{
        results.process_size_ = 416;
        break;
    }
    case object_detect_model_enum::yolo_v8_n:{
        results.process_size_ = ui->comboBoxProcessSizeYoloV8->currentText().toInt();
        break;
    }
    }

    return results;
}

QJsonObject widget_object_detect_model_select::get_states() const
{
    QJsonObject obj;
    obj[state_obj_det_confidence] = ui->spinBoxConfidence->value();
    obj[state_obj_det_nms] = ui->spinBoxNMS->value();
    obj[state_obj_det_model_type] = model_types().get_type(ui->comboBoxSelectModel->currentIndex());
    obj[state_obj_det_yolov8_process_size] = ui->comboBoxProcessSizeYoloV8->currentIndex();
    obj[state_version] = "1.0";

    return obj;
}

void widget_object_detect_model_select::set_states(const QJsonObject &val)
{
    if(val.contains(state_obj_det_confidence)){
        ui->spinBoxConfidence->setValue(val[state_obj_det_confidence].toInt());
    }
    if(val.contains(state_obj_det_model_type)){
        //set_model_index(model_types().get_ids(val[state_obj_det_model_type].toString()));
        ui->comboBoxSelectModel->setCurrentIndex(model_types().get_ids(val[state_obj_det_model_type].toString()));
    }
    if(val.contains(state_obj_det_nms)){
        ui->spinBoxNMS->setValue(val[state_obj_det_nms].toInt());
    }
    if(val.contains(state_obj_det_yolov8_process_size)){
        ui->comboBoxProcessSizeYoloV8->setCurrentIndex(val[state_obj_det_yolov8_process_size].toInt());
    }
}

void widget_object_detect_model_select::set_model_index(int idx) noexcept
{
    switch(static_cast<object_detect_model_enum>(idx)){
    case object_detect_model_enum::nanodet_plus_m_320:
    case object_detect_model_enum::nanodet_plus_m_416:{
        ui->groupBoxYoloV8->setVisible(false);
        break;
    }
    case object_detect_model_enum::yolo_v8_n:{
        ui->groupBoxYoloV8->setVisible(true);
        break;
    }
    }
}
