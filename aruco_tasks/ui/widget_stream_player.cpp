#include "widget_stream_player.hpp"
#include "ui_widget_stream_player.h"

#include "../algo/aruco_detect_worker_results.hpp"

#include <QPixmap>
#include <QTime>

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
    auto const val = std::any_cast<aruco_detect_worker_results>(results);
    int const w = ui->labelStream->width();
    int const h = ui->labelStream->height();

    ui->labelStream->setPixmap(QPixmap::fromImage(val.qimg_).scaled(w,h,Qt::KeepAspectRatio));
}
