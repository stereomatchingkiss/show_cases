#include "widget_image_player.hpp"
#include "ui_widget_image_player.h"

#include "../algo/aruco_detect_worker_results.hpp"

#include "../global/global_object.hpp"

#include <QFileDialog>
#include <QMessageBox>

widget_image_player::widget_image_player(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::widget_image_player)
{
    ui->setupUi(this);
}

widget_image_player::~widget_image_player()
{
    delete ui;
}

void widget_image_player::display_frame(std::any input)
{
    auto const aruco_results = std::any_cast<aruco_detect_worker_results>(input);

    int const w = ui->labelImage->width();
    int const h = ui->labelImage->height();

    ui->labelImage->setPixmap(QPixmap::fromImage(aruco_results.qimg_).scaled(w, h, Qt::KeepAspectRatio));
}

void widget_image_player::on_pushButtonSelectImage_clicked()
{
#ifndef WASM_BUILD
    if(auto const fname = QFileDialog::getOpenFileName(this, tr("Select image"), "", tr("Image (*.jpg *.jpeg *.png *.tiff *.bmp)"));
        !fname.isEmpty() && QFile(fname).exists())
    {
        if(QImage img(fname); !img.isNull()){
            emit image_selected(std::move(img));
        }else{
            get_gobject().messagebox().warning(this, tr("Warning"), tr("Cannot open image %1").arg(fname));
        }
    }
#else
    auto func = [this](QString const &fname, QByteArray const &fcontent) {
        if(auto img = QImage::fromData(fcontent); !img.isNull()){
            qDebug()<<"image can select";
            emit image_selected(img);
        }else{
            get_gobject().messagebox().warning(this, tr("Warning"), tr("Cannot open image %1").arg(fname));
        }
    };
    QFileDialog::getOpenFileContent(tr("Image (*.jpg *.jpeg *.png *.tiff *.bmp)"),  func);
#endif
}

