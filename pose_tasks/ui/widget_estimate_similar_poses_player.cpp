#include "widget_estimate_similar_poses_player.hpp"
#include "ui_widget_estimate_similar_poses_player.h"

#include "../algo/estimate_many_pose_similarity_worker_results.hpp"

#include "../global/global_object.hpp"

#include <cv_algo/pose/pose_similarity_search_results.hpp>
#include <utils/image_utils.hpp>

#include <QFileDialog>
#include <QMessageBox>

#include <QJsonDocument>
#include <QJsonObject>

namespace{

QString to_json(QImage const &img)
{
    QJsonObject obj;
    obj["im"] = QString(flt::to_base64_img(img));
    obj["mode"] = "compare";

    return QJsonDocument(obj).toJson();
}

}

using namespace flt::cvt::pose;

widget_estimate_similar_poses_player::widget_estimate_similar_poses_player(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::widget_estimate_similar_poses_player)
{
    ui->setupUi(this);
}

widget_estimate_similar_poses_player::~widget_estimate_similar_poses_player()
{
    delete ui;
}

void widget_estimate_similar_poses_player::display_frame(std::any input)
{
    auto const val = std::any_cast<estimate_many_pose_similarity_worker_results>(input);
    int const w = ui->labelSourceImage->width();
    int const h = ui->labelSourceImage->height();

    ui->labelSourceImage->setPixmap(QPixmap::fromImage(val.qimg_).scaled(w,h,Qt::KeepAspectRatio));
}

void widget_estimate_similar_poses_player::set_label_text(QString const &text)
{
    ui->labelSourceImage->clear();
    ui->labelSourceImage->setText(text);
}

void widget_estimate_similar_poses_player::set_similar_pose(std::any input)
{
    qDebug()<<"set similar pose";
    auto const results = std::any_cast<std::tuple<pose_similarity_search_results, QImage>>(input);
    auto const pose_info = std::get<0>(results);
    ui->tableWidget->setRowCount(static_cast<int>(pose_info.similarity_.size()));
    for(size_t i = 0; i != pose_info.similarity_.size(); ++i){
        auto const idx = static_cast<int>(i);
        ui->tableWidget->setItem(idx, 0, new QTableWidgetItem(pose_info.img_paths_[i].c_str()));
        ui->tableWidget->setItem(idx, 1, new QTableWidgetItem(QString("%1").arg(pose_info.similarity_[i])));
    }

    ui->tableWidget->resizeColumnsToContents();

    int const w = ui->labelSourceImage->width();
    int const h = ui->labelSourceImage->height();

    QImage const qimg = std::get<1>(results);
    ui->labelSourceImage->setPixmap(QPixmap::fromImage(qimg).scaled(w,h,Qt::KeepAspectRatio));
}

void widget_estimate_similar_poses_player::on_pushButtonSourceImage_clicked()
{
#ifndef WASM_BUILD
    if(auto const fname = QFileDialog::getOpenFileName(this, tr("Select image"), "", tr("Image (*.jpg *.jpeg *.png *.tiff *.bmp)"));
        !fname.isEmpty() && QFile(fname).exists())
    {
        QImage img;
        if(img.load(fname); !img.isNull()){
            emit image_selected(to_json(img));
        }else{
            get_gobject().messagebox().warning(this, tr("Warning"), tr("Cannot open image %1").arg(fname));
        }
    }
#else
    auto func = [this](QString const &fname, QByteArray const &fcontent) {
        QImage img;
        if(img = QImage::fromData(fcontent); !img.isNull()){
            qDebug()<<"image can select";
            emit image_selected(to_json(img));
        }else{
            get_gobject().messagebox().warning(this, tr("Warning"), tr("Cannot open image %1").arg(fname));
        }
    };
    QFileDialog::getOpenFileContent(tr("Image (*.jpg *.jpeg *.png *.tiff *.bmp)"),  func);
#endif
}

