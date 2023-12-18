#include "widget_stream_player.hpp"
#include "ui_widget_stream_player.h"

#include "../algo/generic_worker_results.hpp"
#include "../global/global_object.hpp"

#include <multimedia/sound/alert_sound_manager.hpp>

#include <QPixmap>
#include <QTime>

namespace{

inline QTime secs_to_qtime(qint64 duration_sec)
{
    return QTime((duration_sec / 3600) % 60,
                 (duration_sec / 60) % 60,
                 duration_sec % 60);
}

}

widget_stream_player::widget_stream_player(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::widget_stream_player)
{
    ui->setupUi(this);

    ui->labelStream->setAlignment(Qt::AlignCenter);

    ui->labelCurrentPosition->setText("--:--");
    ui->labelMaxPosition->setText(ui->labelCurrentPosition->text());

    connect(ui->horizontalSliderPosition, &QSlider::sliderMoved, this, &widget_stream_player::seek);

    ui->pushButtonPlayOrPause->setText(tr("Pause"));
    playing_ = true;
    connect(ui->pushButtonPlayOrPause, &QPushButton::clicked, [this](bool)
            {
        if(playing_){
            ui->pushButtonPlayOrPause->setText(tr("Play"));
            emit pause();
        }else{
            ui->pushButtonPlayOrPause->setText(tr("Pause"));
            emit play();
        }
        playing_ = !playing_;
    });
}

widget_stream_player::~widget_stream_player()
{
    delete ui;
}

void widget_stream_player::display_frame(std::any results)
{
    auto const val = std::any_cast<generic_worker_results>(results);
    int const w = ui->labelStream->width();
    int const h = ui->labelStream->height();

    auto qimg = val.mat_;
    ui->labelStream->setPixmap(QPixmap::fromImage(qimg).scaled(w,h,Qt::KeepAspectRatio));

    if(val.alarm_on_){
        get_alert_sound_manager().play();
    }
}

void widget_stream_player::set_is_seekable(bool val)
{
    ui->groupBoxStreamController->setVisible(val);
}

void widget_stream_player::set_current_position(qint64 val)
{
    ui->horizontalSliderPosition->setValue(val);
    ui->labelCurrentPosition->setText(secs_to_qtime(val / 1000).toString(format_));
}

void widget_stream_player::set_duration(qint64 current_duration, qint64 max_duration)
{
    ui->horizontalSliderPosition->setRange(0, max_duration);
    if(current_duration || max_duration) {
        current_duration = current_duration / 1000;
        max_duration = max_duration / 1000;
        format_ = max_duration < 3600 ? "mm:ss" : "hh:mm:ss";
        ui->labelCurrentPosition->setText(secs_to_qtime(current_duration).toString(format_));
        ui->labelMaxPosition->setText(secs_to_qtime(max_duration).toString(format_));
    }
}
