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
#include <QTimer>

#include <QResizeEvent>

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

QString const state_display_details("state_display_details");
QString const state_draw_all("state_draw_all");
QString const state_hide_image("state_hide_image");
QString const state_hide_table("state_hide_table");

}

widget_stream_player::widget_stream_player(QWidget *parent) :
    QWidget(parent)
    ,ui(new Ui::widget_stream_player)
    ,dialog_display_details_{new dialog_display_details(this)}
#ifdef WASM_BUILD
    ,text_rec_("ch_PP-OCRv4_rec_infer.onnx",
                "paddleocr_keys.txt",
                48,
                400)
#endif
{
    ui->setupUi(this);

    ui->labelStream->setAlignment(Qt::AlignCenter);
    ui->labelTextArea->setAlignment(Qt::AlignCenter);

    pen_.setColor(Qt::yellow);
    pen_.setWidth(3);

    pen_all_.setColor(Qt::green);
    pen_all_.setWidth(3);

    font_.setFamily(get_gobject().font_family());
    qDebug()<<"font family is = "<<font_;
    ui->tableWidgetOcrResult->setFont(font_);

#ifdef WASM_BUILD    
    timer_ = new QTimer(this);
    timer_->setInterval(50);
    timer_->setSingleShot(true);
    connect(timer_, &QTimer::timeout, this, &widget_stream_player::update_rec_result);    
#endif

    ui->labelProgress->setVisible(false);
    ui->progressBar->setVisible(false);

    connect(dialog_display_details_, &QDialog::accepted, this, &widget_stream_player::update_table_headers);
    connect(dialog_display_details_, &QDialog::accepted, this, &widget_stream_player::update_table_contents);

    connect(ui->tableWidgetOcrResult, &QTableWidget::cellClicked, this, &widget_stream_player::cell_cliked);

    update_table_headers();
}

widget_stream_player::~widget_stream_player()
{
    delete ui;
}

#ifdef WASM_BUILD
void widget_stream_player::update_rec_result()
{    
    if(text_rec_.predict_results_available()){
        if(process_rec_index_ < text_boxes_.size()){
            text_rec_.predict(text_boxes_[process_rec_index_]);
        }
        ++process_rec_index_;
        ui->labelProgress->setText(QString("%1/%2").arg(QString::number(process_rec_index_), QString::number(text_boxes_.size())));
        ui->progressBar->setValue(process_rec_index_);
        if(process_rec_index_ >= text_boxes_.size()){
            beautify_text_boxes(text_boxes_);
            cv_mat_ = cv::Mat();
            updated_text_rec_results();            
        }else{
            text_rec_.async_predict(cv_mat_, text_boxes_[process_rec_index_]);
            timer_->start();
        }
    }else{
        timer_->start();
    }
}
#endif

void widget_stream_player::updated_text_rec_results()
{
    if(!ui->checkBoxHideTable->isChecked()){
        update_table_headers();
        update_table_contents();
    }

    if(ui->checkBoxDrawAll->isChecked()){
        draw_all();
    }

    if(!can_save_on_local_){
        emit send_ocr_results(QJsonDocument(text_boxes_to_json()).toJson(QJsonDocument::Compact));
    }

    ui->labelProgress->setVisible(false);
    ui->progressBar->setVisible(false);

    emit process_done();
}

void widget_stream_player::display_frame(std::any results)
{
    auto val = std::any_cast<paddle_ocr_worker_results>(results);    
    qimg_ = std::move(val.mat_);    
    text_boxes_ = std::move(val.text_boxes_);
    
    last_clicked_row_ = -1;

    if(!ui->checkBoxHideImage->isChecked() && !qimg_.isNull()){
        update_image_label(qimg_);
    }

#ifdef WASM_BUILD
    process_rec_index_ = 0;
    cv_mat_ = val.cv_mat_;
    if(!text_boxes_.empty()){
        ui->labelProgress->setText(QString("%1/%2").arg(QString::number(process_rec_index_), QString::number(text_boxes_.size())));
        ui->progressBar->setMaximum(text_boxes_.size());    
        ui->labelProgress->setVisible(true);
        ui->progressBar->setVisible(true);    
        text_rec_.async_predict(cv_mat_, text_boxes_[process_rec_index_]);
        timer_->start();
    }else{
        emit process_done();
    }
#endif    

#ifndef WASM_BUILD
    updated_text_rec_results();    
#endif    
}

QJsonObject widget_stream_player::get_states() const
{
    QJsonObject states;
    states[state_display_details] = dialog_display_details_->get_states();
    states[state_draw_all] = ui->checkBoxDrawAll->isChecked();
    states[state_hide_image] = ui->checkBoxHideImage->isChecked();
    states[state_hide_table] = ui->checkBoxHideTable->isChecked();

    return states;
}

void widget_stream_player::set_can_save_on_local(bool val)
{
    can_save_on_local_ = val;
    ui->pushButtonSelectImage->setVisible(can_save_on_local_);
    ui->pushButtonSave->setVisible(can_save_on_local_);
}

void widget_stream_player::set_states(const QJsonObject &val)
{
    if(val.contains(state_display_details)){
        dialog_display_details_->set_states(val[state_display_details].toObject());
    }

    if(val.contains(state_draw_all)){
        ui->checkBoxDrawAll->setChecked(val[state_draw_all].toBool());
    }

    if(val.contains(state_hide_image)){
        ui->checkBoxHideImage->setChecked(val[state_hide_image].toBool());
    }

    if(val.contains(state_hide_table)){
        ui->checkBoxHideTable->setChecked(val[state_hide_table].toBool());
    }
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
            update_image_label(qimg_);
        }
    }

    emit resize_window();
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

    emit resize_window();
}

void widget_stream_player::cell_cliked(int row, int)
{
    last_clicked_row_ = row;
    update_clicked_contents(last_clicked_row_);
}

void widget_stream_player::draw_all()
{
    if(!qimg_.isNull()){
        auto qimg = qimg_.copy();
        QPainter painter(&qimg);
        painter.setPen(pen_all_);
        font_.setPixelSize(qimg.width() * 10);
        painter.setFont(font_);
        for(size_t i = 0; i != text_boxes_.size(); ++i){
            QPolygon poly = text_box_to_qpolygon(static_cast<int>(i));
            painter.drawPolygon(poly);
            poly[0].setY(std::max(0, poly[0].y() - 5));
            painter.drawText(poly[0], text_boxes_[i].text.c_str());
        }

        int const w = ui->labelStream->width();
        int const h = ui->labelStream->height();
        ui->labelStream->setPixmap(QPixmap::fromImage(qimg).scaled(w, h, Qt::KeepAspectRatio));
    }
}

QPolygon widget_stream_player::text_box_to_qpolygon(int row) const
{
    QPolygon poly;
    for(auto const &pt : text_boxes_[row].boxPoint){
        poly<<QPoint(pt.x, pt.y);
    }

    return poly;
}

QJsonObject widget_stream_player::text_boxes_to_json() const
{
    QJsonArray arr;
    for(auto const &val : text_boxes_){
        QJsonObject obj;
        obj["text"] = val.text.c_str();

        obj["tl"] = std::format("{},{}", val.boxPoint[0].x, val.boxPoint[0].y).c_str();
        obj["tr"] = std::format("{},{}", val.boxPoint[1].x, val.boxPoint[1].y).c_str();
        obj["br"] = std::format("{},{}", val.boxPoint[2].x, val.boxPoint[2].y).c_str();
        obj["bl"] = std::format("{},{}", val.boxPoint[3].x, val.boxPoint[3].y).c_str();

        obj["confidence"] = val.score;
        arr.push_back(obj);
    }

    QJsonObject result;
    result["contents"] = arr;

    return result;
}

void widget_stream_player::update_clicked_contents(int row)
{
    if(!qimg_.isNull()){
        if(!ui->checkBoxHideTable->isChecked() && ui->checkBoxHideImage->isChecked()){
            ui->labelTextArea->setVisible(true);

            auto brect= text_box_to_qpolygon(row).boundingRect();
            QPoint clip_pt;
            clip_pt.setX(std::min(brect.x() + brect.width(), qimg_.width() - 1));
            clip_pt.setY(std::min(brect.y() + brect.height(), qimg_.height() - 1));
            brect.setBottomRight(clip_pt);
            auto qimg = qimg_.copy(brect);
            update_image_label(qimg);
        }

        if(!ui->checkBoxHideImage->isChecked()){
            auto qimg_copy = qimg_.copy();
            QPainter painter(&qimg_copy);
            painter.setPen(pen_);
            painter.drawPolygon(text_box_to_qpolygon(row));

            update_image_label(qimg_copy);
        }
    }
}

void widget_stream_player::update_image_label(const QImage &img)
{
    if(!img.isNull()){
        int const w = ui->labelStream->width();
        int const h = std::min(ui->labelStream->height(), height());
        ui->labelStream->setPixmap(QPixmap::fromImage(img).scaled(w, h, Qt::KeepAspectRatio));
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
                auto const &pt = text_boxes_[i].boxPoint[j - table_tag::tl];
                item->setText(std::format("{},{}", pt.x, pt.y).c_str());
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


void widget_stream_player::on_checkBoxDrawAll_clicked()
{
    if(ui->checkBoxDrawAll->isChecked()){
        draw_all();
    }else{
        update_image_label(qimg_);
    }

    emit resize_window();
}
