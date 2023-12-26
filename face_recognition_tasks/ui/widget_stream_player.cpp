#include "widget_stream_player.hpp"
#include "ui_widget_stream_player.h"

#include "../algo/face_search/face_search_worker_results.hpp"

#include <QJsonDocument>

#include <QPixmap>

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
    auto const val = std::any_cast<face_search_worker_results>(results);
    int const w = ui->labelStream->width();
    int const h = ui->labelStream->height();

    if(!val.img.isNull()){
        ui->labelStream->setPixmap(QPixmap::fromImage(val.img).scaled(w, h, Qt::KeepAspectRatio));
    }

    emit send_text_msg(QJsonDocument(val.obj).toJson(QJsonDocument::Indented));
}
