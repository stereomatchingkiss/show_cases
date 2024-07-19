#include "widget_object_detect_model_select.hpp"
#include "ui_widget_object_detect_model_select.h"

#include "../../config/config_object_detect_model_select.hpp"

#include <QJsonObject>
#include <QLineEdit>

namespace{

inline QString state_obj_det_confidence(){ return "state_obj_det_confidence"; };
inline QString state_obj_det_model_type(){ return "state_obj_det_model_type"; };
inline QString state_obj_det_nms(){ return "state_obj_det_nms"; };

inline QString state_version(){ return "state_version"; };

struct model_types
{
    QString const nanodet_plus_m_320_ = "nanodet_plus_m_320";
    QString const nanodet_plus_m_416_ = "nanodet_plus_m_416";
    QString const nanodet_plus_m_1_5x_320_ = "nanodet_plus_m_1_5x_320";
    QString const nanodet_plus_m_1_5x_416_ = "nanodet_plus_m_1_5x_416";
    QString const yolo_v8_n_416 = "yolo_v8_n_416";

    int get_ids(QString const &val) const
    {
        if(val == nanodet_plus_m_320_){
            return 0;
        }

        if(val == nanodet_plus_m_416_){
            return 1;
        }

        if(val == nanodet_plus_m_1_5x_320_){
            return 2;
        }

        if(val == nanodet_plus_m_1_5x_416_){
            return 3;
        }

        if(val == yolo_v8_n_416){
            return 4;
        }

        return 1;
    }
};

}

widget_object_detect_model_select::widget_object_detect_model_select(QWidget *parent) :
    QWidget(parent),
    ui(new Ui_widget_object_detect_model_select)
{
    ui->setupUi(this);

    model_types const mt;
    ui->comboBoxSelectModel->addItem(mt.nanodet_plus_m_320_);
    ui->comboBoxSelectModel->addItem(mt.nanodet_plus_m_416_);
    ui->comboBoxSelectModel->addItem(mt.nanodet_plus_m_1_5x_320_);
    ui->comboBoxSelectModel->addItem(mt.nanodet_plus_m_1_5x_416_);
    ui->comboBoxSelectModel->addItem(mt.yolo_v8_n_416);
    ui->comboBoxSelectModel->setCurrentIndex(1);

    ui->groupBoxYolox->setVisible(false);

    ui->labelConfidence->setToolTip(tr("The higher the value, the more likely the model is to correctly identify the correct results.\n"
                                       "However, this can also lead to fewer of the correct results being identified."));
    ui->labelNMS->setToolTip(tr("The larger the NMS value, the fewer objects will be filtered.\n"
                                "If you find that there are many repetitive objects detected under a crowded scene, "
                                "please reduce this value."));

    ui->spinBoxConfidence->setValue(25);
}

widget_object_detect_model_select::~widget_object_detect_model_select()
{
    delete ui;
}

config_object_detect_model_select widget_object_detect_model_select::get_config() const
{
    config_object_detect_model_select results;
    results.confidence_ = static_cast<float>(ui->spinBoxConfidence->value()) / 100.0f;
    results.model_ = static_cast<object_detect_model_enum>(model_types().get_ids(ui->comboBoxSelectModel->currentText()));
    results.nms_ = static_cast<float>(ui->spinBoxNMS->value()) / 100.0f;

    model_types const mt;
    auto const mtext = ui->comboBoxSelectModel->currentText();
    if(mtext == mt.nanodet_plus_m_1_5x_320_ || mtext == mt.nanodet_plus_m_320_){
        results.process_size_ = 320;
    }else{
        results.process_size_ = 416;
    }

    return results;
}

QJsonObject widget_object_detect_model_select::get_states() const
{
    QJsonObject obj;
    obj[state_obj_det_confidence()] = ui->spinBoxConfidence->value();
    obj[state_obj_det_nms()] = ui->spinBoxNMS->value();
    obj[state_obj_det_model_type()] = ui->comboBoxSelectModel->currentText();
    obj[state_version()] = "1.0";

    return obj;
}

void widget_object_detect_model_select::set_states(const QJsonObject &val)
{
    if(val.contains(state_obj_det_confidence())){
        ui->spinBoxConfidence->setValue(val[state_obj_det_confidence()].toInt());
    }
    if(val.contains(state_obj_det_model_type())){        
        ui->comboBoxSelectModel->setCurrentIndex(model_types().get_ids(val[state_obj_det_model_type()].toString()));
    }
    if(val.contains(state_obj_det_nms())){
        ui->spinBoxNMS->setValue(val[state_obj_det_nms()].toInt());
    }    
}

