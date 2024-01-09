#include "widget_stream_player.hpp"
#include "ui_widget_stream_player.h"

#include "../config/config_source_selection.hpp"

#include "../global/global_object.hpp"

#include <QFileDialog>
#include <QMessageBox>

#include <QPixmap>

using stype = flt::mm::stream_source_type;

widget_stream_player::widget_stream_player(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::widget_stream_player)
{
    ui->setupUi(this);

    ui->labelStream->setAlignment(Qt::AlignCenter);    
}

widget_stream_player::~widget_stream_player()
{
    delete ui;
}

void widget_stream_player::display_frame(std::any results)
{
    auto const img = std::any_cast<QImage>(results);
    int const w = ui->labelStream->width();
    int const h = ui->labelStream->height();

    if(!img.isNull()){
        ui->labelStream->setPixmap(QPixmap::fromImage(img).scaled(w, h, Qt::KeepAspectRatio));
    }    
}

void widget_stream_player::set_source_type(flt::mm::stream_source_type source_type)
{
    switch(source_type){
    case stype::image:{
        ui->pushButtonSelectImage->setVisible(true);
        break;
    }
    default:{
        ui->pushButtonSelectImage->setVisible(false);
        break;
    }
    }
}

void widget_stream_player::on_pushButtonSelectImage_clicked()
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

