#include "widget_stream_player.hpp"
#include "ui_widget_stream_player.h"

#include "../algo/paddle_ocr_worker_results.hpp"

#include "../global/global_object.hpp"

#include "dialog_display_details.hpp"

#include <QFileDialog>
#include <QMessageBox>

#include <QFile>
#include <QTextStream>

#include <QFontDatabase>

#include <QDebug>
#include <QImage>
#include <QPixmap>

#include <opencv2/highgui.hpp>

widget_stream_player::widget_stream_player(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::widget_stream_player),
    dialog_display_details_{new dialog_display_details(this)}
{
    ui->setupUi(this);

    ui->labelStream->setAlignment(Qt::AlignCenter);

#ifdef WASM_BUILD
    if(int const id = QFontDatabase::addApplicationFont("GB2312.ttf"); id >= 0){
        QString const family = QFontDatabase::applicationFontFamilies(id).at(0);
        ui->textEditOcrResult->setFontFamily(family);
    }
#endif
}

widget_stream_player::~widget_stream_player()
{
    delete ui;
}

void widget_stream_player::display_frame(std::any results)
{
    auto const val = std::any_cast<paddle_ocr_worker_results>(results);
    int const w = ui->labelStream->width();
    int const h = ui->labelStream->height();

    auto qimg = val.mat_;
    ui->labelStream->setPixmap(QPixmap::fromImage(qimg).scaled(w,h,Qt::KeepAspectRatio));

    QString output;
    for(auto const &results : val.text_boxes_){
        output += (results.text + "\n").c_str();
    }

    ui->textEditOcrResult->clear();
    ui->textEditOcrResult->setPlainText(output);
}

void widget_stream_player::set_can_save_on_local(bool val)
{
    can_save_on_local_ = val;
}

void widget_stream_player::on_pushButtonSave_clicked()
{
    if(can_save_on_local_){
#ifndef WASM_BUILD
        if(auto const fname = QFileDialog::getSaveFileName(this, tr("Save as"));
            !fname.isEmpty() && QFile(fname).exists())
        {
            if(QFile file(fname); file.open(QIODevice::WriteOnly)){
                QTextStream stream(&file);
                stream << ui->textEditOcrResult->toPlainText();
            }
        }
#else
        QFileDialog::saveFileContent(ui->textEditOcrResult->toPlainText().toUtf8(), "ocr_results.json");
#endif
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
            global_get_messagebox().warning(this, tr("Warning"), tr("Cannot open image %1").arg(fname));
        }
    }
#else
    auto func = [this](QString const&, QByteArray const &fcontent) {
        if(auto img = QImage::fromData(fcontent); !img.isNull()){
            qDebug()<<"image can select";
            emit image_selected(img);
        }else{
            global_get_messagebox().warning(this, tr("Warning"), tr("Cannot open image %1").arg(fname));
        }
    };
    QFileDialog::getOpenFileContent(tr("Image (*.jpg *.jpeg *.png *.tiff *.bmp)"),  func);
#endif
}


void widget_stream_player::on_pushButtonDisplayDetails_clicked()
{
    dialog_display_details_->show();
}

