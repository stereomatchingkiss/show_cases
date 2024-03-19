#include "widget_image_pair_player.hpp"
#include "ui_widget_image_pair_player.h"

#include "../algo/estimate_pose_similarity_results.hpp"
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
    auto const results = std::any_cast<estimate_pose_similarity_results>(input);
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
    load_image(source_img_);
    if(!source_img_.isNull()){
        emit source_image_selected(source_img_);
    }
}

void widget_image_pair_player::on_pushButtonTargetImage_clicked()
{
    load_image(target_img_);
    if(!target_img_.isNull()){
        emit target_image_selected(target_img_);
    }
}

void widget_image_pair_player::load_image(QImage &output)
{
#ifndef WASM_BUILD
    if(auto const fname = QFileDialog::getOpenFileName(this, tr("Select image"), "", tr("Image (*.jpg *.jpeg *.png *.tiff *.bmp)"));
        !fname.isEmpty() && QFile(fname).exists())
    {
        if(output.load(fname); output.isNull()){
            get_gobject().messagebox().warning(this, tr("Warning"), tr("Cannot open image %1").arg(fname));
        }
    }
#else
    auto func = [this](QString const &fname, QByteArray const &fcontent) {
        if(origin_img_ = QImage::fromData(fcontent); origin_img_.isNull()){
            get_gobject().messagebox().warning(this, tr("Warning"), tr("Cannot open image %1").arg(fname));
        }
    };
    QFileDialog::getOpenFileContent(tr("Image (*.jpg *.jpeg *.png *.tiff *.bmp)"),  func);
#endif
}

void widget_image_pair_player::set_image(const QImage &img, QLabel *label)
{
    label->setPixmap(QPixmap::fromImage(img).scaled(label->width(), label->height(), Qt::KeepAspectRatio));
}

