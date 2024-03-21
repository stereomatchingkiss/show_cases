#include "widget_image_pair_player.hpp"
#include "ui_widget_image_pair_player.h"

#include "../algo/estimate_pose_similarity_worker_results.hpp"
#include "../global/global_object.hpp"

#include <QFileDialog>
#include <QMessageBox>

widget_image_pair_player::widget_image_pair_player(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::widget_image_pair_player)
{
    ui->setupUi(this);

    ui->labelSourceImage->setAlignment(Qt::AlignCenter);
    ui->labelTargetImage->setAlignment(Qt::AlignCenter);
}

widget_image_pair_player::~widget_image_pair_player()
{
    delete ui;
}

void widget_image_pair_player::display_frame(std::any input)
{
    auto const results = std::any_cast<estimate_pose_similarity_worker_results>(input);
    if(results.is_target_){
        set_image(results.qimg_, ui->labelTargetImage);
    }else{
        set_image(results.qimg_, ui->labelSourceImage);
    }

    if(results.im_similarity_ >= 0){
        ui->labelInfo->setText("Euclidean distance = " + QString::number(results.im_similarity_) +
                               tr("\nThe lower the value, the higher the similarity"));
    }
}

void widget_image_pair_player::on_pushButtonSourceImage_clicked()
{    
    load_image(source_img_, true);
}

void widget_image_pair_player::on_pushButtonTargetImage_clicked()
{
    load_image(target_img_, false);
}

void widget_image_pair_player::load_image(QImage &output, bool is_source_image)
{
#ifndef WASM_BUILD
    if(auto const fname = QFileDialog::getOpenFileName(this, tr("Select image"), "", tr("Image (*.jpg *.jpeg *.png *.tiff *.bmp)"));
        !fname.isEmpty() && QFile(fname).exists())
    {
        if(output.load(fname); output.isNull()){
            get_gobject().messagebox().warning(this, tr("Warning"), tr("Cannot open image %1").arg(fname));
        }else{
            if(is_source_image){
                emit source_image_selected(output);
            }else{
                emit target_image_selected(target_img_);
            }
        }
    }
#else
    auto func = [this, &output, is_source_image](QString const &fname, QByteArray const &fcontent) {
        if(output = QImage::fromData(fcontent); output.isNull()){
            get_gobject().messagebox().warning(this, tr("Warning"), tr("Cannot open image %1").arg(fname));
        }else{
            if(is_source_image){
                emit source_image_selected(output);
            }else{
                emit target_image_selected(output);
            }
        }
    };
    QFileDialog::getOpenFileContent(tr("Image (*.jpg *.jpeg *.png *.tiff *.bmp)"),  func);
#endif
}

void widget_image_pair_player::set_image(const QImage &img, QLabel *label)
{
    label->setPixmap(QPixmap::fromImage(img).scaled(label->width(), label->height(), Qt::KeepAspectRatio));
}

