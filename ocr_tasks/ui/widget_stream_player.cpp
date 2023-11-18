#include "widget_stream_player.hpp"
#include "ui_widget_stream_player.h"

#include "../algo/paddle_ocr_worker_results.hpp"

#include "../config/config_dialog_display_details.hpp"

#include "../global/global_object.hpp"

#include "dialog_display_details.hpp"

#include <json/json_utils.hpp>

#include <QFileDialog>
#include <QMessageBox>
#include <QTableWidgetItem>

#include <QFile>
#include <QTextStream>

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

#include <QFontDatabase>

#include <QDebug>
#include <QImage>
#include <QPainter>
#include <QPixmap>

#include <QResizeEvent>

#include <opencv2/highgui.hpp>

#include <format>

namespace{

enum table_tag
{
    text,
    tl,
    tr,
    br,
    bl,
    confidence
};

}

widget_stream_player::widget_stream_player(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::widget_stream_player),
    dialog_display_details_{new dialog_display_details(this)}
{
    ui->setupUi(this);

    ui->labelStream->setAlignment(Qt::AlignCenter);
    ui->labelTextArea->setAlignment(Qt::AlignCenter);

    pen_.setColor(Qt::green);
    pen_.setWidth(3);

    connect(dialog_display_details_, &QDialog::accepted, this, &widget_stream_player::update_table_headers);
    connect(dialog_display_details_, &QDialog::accepted, this, &widget_stream_player::update_table_contents);

    connect(ui->tableWidgetOcrResult, &QTableWidget::cellClicked, this, &widget_stream_player::cell_cliked);

    update_table_headers();

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
    auto val = std::any_cast<paddle_ocr_worker_results>(results);
    int const w = ui->labelStream->width();
    int const h = ui->labelStream->height();

    qimg_ = std::move(val.mat_);
    text_boxes_ = std::move(val.text_boxes_);

    last_clicked_row_ = -1;

    if(!ui->checkBoxHideImage->isChecked()){
        ui->labelStream->setPixmap(QPixmap::fromImage(qimg_).scaled(w, h, Qt::KeepAspectRatio));
    }

    if(!ui->checkBoxHideTable->isChecked()){
        update_table_headers();
        update_table_contents();
    }
}

void widget_stream_player::set_can_save_on_local(bool val)
{
    can_save_on_local_ = val;
}

void widget_stream_player::on_pushButtonSave_clicked()
{
    if(can_save_on_local_){
#ifndef WASM_BUILD
        if(auto const fname = QFileDialog::getSaveFileName(this, tr("Save as")); !fname.isEmpty()){

            if(auto const alert_config = dialog_display_details_->get_config();
                alert_config.show_location_ || alert_config.show_confidence_){
                flt::json::write_file_to_json(table_contents_to_json(), fname);
            }else{
                if(QFile file(fname); file.open(QIODevice::WriteOnly)){
                    QTextStream stream(&file);
                    stream<<table_contents_to_string();
                }
            }
        }
#else        
        if(auto const alert_config = dialog_display_details_->get_config();
            alert_config.show_location_ || alert_config.show_confidence_){
            QFileDialog::saveFileContent(QJsonDocument(table_contents_to_json()).toJson(), "ocr_results.json");
        }else{
            QFileDialog::saveFileContent(table_contents_to_string().toUtf8(), "ocr_results.txt");
        }
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


void widget_stream_player::on_checkBoxHideImage_stateChanged(int)
{
    ui->labelStream->setVisible(!ui->checkBoxHideImage->isChecked());
    if(ui->checkBoxHideImage->isChecked()){
        ui->labelStream->setVisible(false);
        ui->labelStream->clear();
    }else{
        ui->labelStream->setVisible(true);
        ui->labelTextArea->setVisible(false);
        ui->labelTextArea->clear();
        if(!qimg_.isNull()){
            int const w = ui->labelStream->width();
            int const h = ui->labelStream->height();
            ui->labelStream->setPixmap(QPixmap::fromImage(qimg_).scaled(w, h, Qt::KeepAspectRatio));
        }
    }
}


void widget_stream_player::on_checkBoxHideTable_stateChanged(int)
{
    ui->groupBoxTable->setVisible(!ui->checkBoxHideTable->isChecked());
    ui->labelTextArea->setVisible(!ui->checkBoxHideTable->isChecked());
    if(!ui->checkBoxHideTable->isChecked()){
        update_table_headers();
        update_table_contents();
    }else{
        ui->labelTextArea->clear();
    }
}

void widget_stream_player::cell_cliked(int row, int)
{
    last_clicked_row_ = row;
    update_clicked_contents(last_clicked_row_);
}

QPolygon widget_stream_player::text_box_to_qpolygon(int row) const
{
    QPolygon poly;
    for(auto const &pt : text_boxes_[row].boxPoint){
        poly<<QPoint(pt.x, pt.y);
    }

    return poly;
}

void widget_stream_player::update_clicked_contents(int row)
{
    if(!ui->checkBoxHideTable->isChecked() && ui->checkBoxHideImage->isChecked()){
        ui->labelTextArea->setVisible(true);

        auto brect= text_box_to_qpolygon(row).boundingRect();
        QPoint clip_pt;
        clip_pt.setX(std::min(brect.x() + brect.width(), qimg_.width() - 1));
        clip_pt.setY(std::min(brect.y() + brect.height(), qimg_.height() - 1));
        brect.setBottomRight(clip_pt);
        auto qimg = qimg_.copy(brect);
        int const w = ui->labelTextArea->width();
        int const h = std::max(100, ui->labelTextArea->height());
        ui->labelTextArea->setPixmap(QPixmap::fromImage(qimg).scaled(w, h, Qt::KeepAspectRatio));
    }

    if(!ui->checkBoxHideImage->isChecked()){
        auto qimg_copy = qimg_.copy();
        QPainter painter(&qimg_copy);
        painter.setPen(pen_);
        painter.drawPolygon(text_box_to_qpolygon(row));

        int const w = ui->labelStream->width();
        int const h = ui->labelStream->height();
        ui->labelStream->setPixmap(QPixmap::fromImage(qimg_copy).scaled(w, h, Qt::KeepAspectRatio));
    }
}

void widget_stream_player::update_table_headers()
{
    auto const alert_config = dialog_display_details_->get_config();
    if(!alert_config.show_confidence_){
        ui->tableWidgetOcrResult->hideColumn(table_tag::confidence);
    }else{
        ui->tableWidgetOcrResult->showColumn(table_tag::confidence);
    }

    for(int i = table_tag::tl; i <= table_tag::bl; ++i){
        if(!alert_config.show_location_){
            ui->tableWidgetOcrResult->hideColumn(i);
        }else{
            ui->tableWidgetOcrResult->showColumn(i);
        }
    }
}

void widget_stream_player::update_table_contents()
{
    auto const alert_config = dialog_display_details_->get_config();
    ui->tableWidgetOcrResult->setRowCount(static_cast<int>(text_boxes_.size()));
    for(int i = 0; i != static_cast<int>(text_boxes_.size()); ++i){

        QTableWidgetItem *item = new QTableWidgetItem;
        item->setText(text_boxes_[i].text.c_str());
        ui->tableWidgetOcrResult->setItem(i, 0, item);

        if(alert_config.show_confidence_){
            QTableWidgetItem *item = new QTableWidgetItem;
            item->setText(QString::number(text_boxes_[i].score, 'g', 2));
            ui->tableWidgetOcrResult->setItem(i, table_tag::confidence, item);
        }

        if(alert_config.show_location_){
            for(int j = table_tag::tl; j <= table_tag::bl; ++j){
                QTableWidgetItem *item = new QTableWidgetItem;
                item->setText(std::format("{},{}",
                                          text_boxes_[i].boxPoint[j - table_tag::tl].x,
                                          text_boxes_[i].boxPoint[j - table_tag::tl].y).c_str());
                ui->tableWidgetOcrResult->setItem(i, j, item);
            }
        }
    }

    ui->tableWidgetOcrResult->resizeColumnsToContents();
}

QJsonObject widget_stream_player::table_contents_to_json() const
{
    auto const alert_config = dialog_display_details_->get_config();
    QJsonArray arr;
    for(int i = 0; i != ui->tableWidgetOcrResult->rowCount(); ++i){
        QJsonObject obj;
        obj["text"] = ui->tableWidgetOcrResult->item(i, table_tag::text)->text();

        if(alert_config.show_location_){
            obj["tl"] = ui->tableWidgetOcrResult->item(i, table_tag::tl)->text();
            obj["tr"] = ui->tableWidgetOcrResult->item(i, table_tag::tr)->text();
            obj["br"] = ui->tableWidgetOcrResult->item(i, table_tag::br)->text();
            obj["bl"] = ui->tableWidgetOcrResult->item(i, table_tag::bl)->text();
        }

        if(alert_config.show_confidence_){
            obj["confidence"] = ui->tableWidgetOcrResult->item(i, table_tag::confidence)->text();
        }
        arr.push_back(obj);
    }

    QJsonObject result;
    result["contents"] = arr;

    return result;
}

QString widget_stream_player::table_contents_to_string() const
{
    QString result;
    for(int i = 0; i != ui->tableWidgetOcrResult->rowCount(); ++i){
        result += ui->tableWidgetOcrResult->item(i, table_tag::text)->text() + "\n";
    }

    return result;
}

void widget_stream_player::resizeEvent(QResizeEvent *e)
{

    if(last_clicked_row_ != -1){
        update_clicked_contents(last_clicked_row_);
    }
    QWidget::resizeEvent(e);
}

