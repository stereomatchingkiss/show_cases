#include "widget_stream_player.hpp"
#include "ui_widget_stream_player.h"

#include <QPixmap>

widget_stream_player::widget_stream_player(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::widget_stream_player)
{
    ui->setupUi(this);

    ui->labelStream->setScaledContents(true);    
}

widget_stream_player::~widget_stream_player()
{
    delete ui;
}

void widget_stream_player::display_frame(std::any img)
{
    int const w = ui->labelStream->width();
    int const h = ui->labelStream->height();
    ui->labelStream->setPixmap(std::any_cast<QPixmap>(img).scaled(w,h,Qt::KeepAspectRatio));
}
