#include "widget_stream_player.hpp"
#include "ui_widget_stream_player.h"

#include "../algo/anpr_results_drawer.hpp"

#include "../global/global_object.hpp"

#include <multimedia/stream_enum.hpp>

#include <QFileDialog>
#include <QMessageBox>

#include <QPixmap>

#include <format>

using stype = flt::mm::stream_source_type;

namespace{

inline QString cv_rect_to_str(cv::Rect const &rect)
{
    return std::format("{},{},{},{}", rect.x, rect.y, rect.width, rect.height).c_str();
}

}

widget_stream_player::widget_stream_player(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::widget_stream_player)
{
    ui->setupUi(this);

    ui->labelStream->setAlignment(Qt::AlignCenter);

    ui->tableWidget->setVisible(false);
    ui->tableWidget->setFont(QFont(global_object().font_family()));

    connect(ui->tableWidget, &QTableWidget::cellClicked, this, &widget_stream_player::cell_clicked);
}

widget_stream_player::~widget_stream_player()
{
    delete ui;
}

void widget_stream_player::display_frame(std::any results)
{
    results_ = std::any_cast<anpr_worker_results>(results);
    if(!results_.qimg_.isNull()){
        draw_all();
        ui->tableWidget->setRowCount(static_cast<int>(results_.plate_results_.size()));
        for(int i = 0; i != ui->tableWidget->rowCount(); ++i){
            auto const &val = results_.plate_results_[i];
            if(!val.plate_num_.isEmpty() && !val.plate_rect_.empty()){
                ui->tableWidget->setItem(i, 0, new QTableWidgetItem(val.plate_num_));
                ui->tableWidget->setItem(i, 1, new QTableWidgetItem(cv_rect_to_str(val.plate_rect_)));
            }
        }

        ui->tableWidget->resizeColumnsToContents();
    }
}

void widget_stream_player::draw_all()
{
    anpr_results_drawer drawer;
    auto img = results_.qimg_.copy();
    drawer.draw(img, results_.full_results_);

    int const w = ui->labelStream->width();
    int const h = ui->labelStream->height();

    ui->labelStream->setPixmap(QPixmap::fromImage(img).scaled(w, h, Qt::KeepAspectRatio));
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


void widget_stream_player::on_checkBoxShowTable_clicked()
{
    ui->tableWidget->setVisible(ui->checkBoxShowTable->isChecked());
    if(!ui->checkBoxShowTable->isChecked()){
        draw_all();
    }
}

void widget_stream_player::cell_clicked(int row, int)
{
    if(row < results_.plate_results_.size()){
        anpr_results_drawer drawer;
        auto img = results_.qimg_.copy();
        drawer.draw(img, results_.plate_results_[row]);

        int const w = ui->labelStream->width();
        int const h = ui->labelStream->height();

        ui->labelStream->setPixmap(QPixmap::fromImage(img).scaled(w, h, Qt::KeepAspectRatio));
    }
}

