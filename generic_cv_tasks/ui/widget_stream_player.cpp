#include "widget_stream_player.hpp"
#include "ui_widget_stream_player.h"

#include "../algo/obj_detect/obj_det_worker_results.hpp"
#include "../global/global_object.hpp"

#include <multimedia/sound/alert_sound_manager.hpp>

#include <QPixmap>

widget_stream_player::widget_stream_player(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::widget_stream_player)
{
    ui->setupUi(this);    
}

widget_stream_player::~widget_stream_player()
{
    delete ui;
}

void widget_stream_player::display_frame(std::any results)
{
    auto val = std::any_cast<obj_det_worker_results>(results);
    int const w = ui->labelStream->width();
    int const h = ui->labelStream->height();
    ui->labelStream->setPixmap(val.pixmap_.scaled(w,h,Qt::KeepAspectRatio));

    if(val.alarm_on_){
        get_alert_sound_manager().play();
    }
}
