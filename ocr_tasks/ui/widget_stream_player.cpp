#include "widget_stream_player.hpp"
#include "ui_widget_stream_player.h"

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
    /*auto const val = std::any_cast<obj_det_worker_results>(results);
    int const w = ui->labelStream->width();
    int const h = ui->labelStream->height();

    auto qimg = val.mat_;
    ui->labelStream->setPixmap(QPixmap::fromImage(qimg).scaled(w,h,Qt::KeepAspectRatio));//*/
}
