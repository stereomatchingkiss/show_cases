#include "widget_faces_register.hpp"
#include "ui_widget_faces_register.h"

#include "../algo/face_register/face_register_worker.hpp"
#include "../global/global_keywords.hpp"

#include <json/json_utils.hpp>
#include <multimedia/camera/frame_process_controller.hpp>
#include <utils/image_utils.hpp>

#include <QFileDialog>

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

#include <QMessageBox>

#include <QDebug>

using namespace flt::json;
using namespace flt::mm;

widget_faces_register::widget_faces_register(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::widget_faces_register),
    msg_box_(new QMessageBox(this))
{
    ui->setupUi(this);

    ui->pushButtonNext->setVisible(false);
    ui->pushButtonPrev->setVisible(false);

    ui->groupBoxFacesInTheImage->setVisible(fresults_.faces_.empty() == false);
}

widget_faces_register::~widget_faces_register()
{
    delete ui;
}

void widget_faces_register::clear_controller()
{
    controller_.reset();
}

void widget_faces_register::on_pushButtonSelectImage_clicked()
{
    init_controller();

    auto const filter = tr("Images (*.png *.jpeg *.jpg)");
#ifndef WASM_BUILD
    if(auto const fname = QFileDialog::getOpenFileName(this, tr("Select image"), "", filter);
        !fname.isEmpty() && QFile(fname).exists())
    {
        if(QImage img(fname); !img.isNull()){
            controller_->predict(img);
            controller_->stop(false);
            emit controller_->start();
        }else{
            msg_box_->warning(this, tr("Warning"), tr("Cannot load the file %1").arg(fname));
        }
    }
#else
    auto fcontent_ready = [this](QString const &fname, QByteArray const &fcontent) {
        if(auto img = QImage::fromData(fcontent); !img.isNull()){
            controller_->predict(img);
            emit controller_->start();
        }else{
            msg_box_->warning(this, tr("Warning"), tr("Cannot load the file %1").arg(fname));
        }
    };
    QFileDialog::getOpenFileContent(filter,  fcontent_ready);
#endif
}


void widget_faces_register::on_pushButtonPrev_clicked()
{
    if(face_index_ > 0){
        --face_index_;
    }

    if(face_index_ == 0){
        ui->pushButtonPrev->setVisible(false);
    }

    if(fresults_.faces_.size() > 1){
        ui->pushButtonNext->setVisible(true);
    }
    display_face();
}

void widget_faces_register::on_pushButtonNext_clicked()
{
    ++face_index_;
    if(face_index_ == fresults_.faces_.size() - 1){
        ui->pushButtonNext->setVisible(false);
    }

    ui->pushButtonPrev->setVisible(true);
    display_face();
}


void widget_faces_register::on_pushButtonAdd_clicked()
{
    init_controller();



#ifndef WASM_BUILD
    if(auto const fname = QFileDialog::getSaveFileName(this, tr("Save at")); !fname.isEmpty())
    {
        auto const obj = generate_save_content();
        if(obj.empty()){
            msg_box_->warning(this, tr("Warning"), tr("Cannot save the contents"));
        }else{
            write_file_to_json(generate_save_content(), fname);
        }
    }
#else
    auto const obj = generate_save_content();
    if(obj.empty()){
        msg_box_->warning(this, tr("Warning"), tr("Cannot save the contents"));
    }else{
        QFileDialog::saveFileContent(QJsonDocument(obj).toJson(), "");
    }
#endif
}

QJsonObject widget_faces_register::generate_save_content() const
{
    QJsonObject obj;
    if(fresults_.faces_.size() > 0 && face_index_ >= 0 && face_index_ < fresults_.faces_.size()){
        QJsonArray features;
        for(auto const &val : fresults_.features_[face_index_]){
            features.push_back(val);
        }

        global_keywords const gk;
        obj[gk.face_features()] = features;
        obj[gk.face_image()] = QString(flt::to_base64_img(ui->labelFace->pixmap().toImage()));
    }

    return obj;
}

void widget_faces_register::display_face()
{
    if(fresults_.faces_.size() > 0 && face_index_ >= 0 && face_index_ < fresults_.faces_.size()){
        ui->labelFace->setPixmap(QPixmap::fromImage(fresults_.faces_[face_index_]));
    }
}

void widget_faces_register::init_controller()
{
    if(!controller_){
        auto worker = new face_register_worker;
        controller_ = std::make_unique<frame_process_controller>(worker);

        connect(worker, &face_register_worker::send_process_results, this, &widget_faces_register::process_results);
    }
}

void widget_faces_register::process_results(std::any results)
{
    fresults_ = std::any_cast<face_register_results>(results);
    face_index_ = 0;
    if(fresults_.faces_.size() > 1){
        ui->pushButtonNext->setVisible(true);
        ui->pushButtonPrev->setVisible(false);
    }else{
        ui->pushButtonNext->setVisible(false);
        ui->pushButtonPrev->setVisible(false);
    }

    ui->groupBoxFacesInTheImage->setVisible(fresults_.faces_.empty() == false);

    controller_->stop(true);
    display_face();

    qDebug()<<"fresults_.faces_.size() = "<<fresults_.faces_.size();
}

