#include "widget_stacks_aruco_stitch.hpp"
#include "ui_widget_stacks_aruco_stitch.h"

#include "dialog_aruco_detector_params.hpp"
#include "widget_stream_player.hpp"
#include "widget_source_selection.hpp"

#include <multimedia/camera/frame_process_controller.hpp>
#include <multimedia/network/frame_capture_websocket.hpp>
#include <multimedia/network/frame_capture_websocket_params.hpp>
#include <multimedia/sound/alert_sound_manager.hpp>
#include <multimedia/stream_enum.hpp>

widget_stacks_aruco_stitch::widget_stacks_aruco_stitch(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::widget_stacks_aruco_stitch)
{
    ui->setupUi(this);
}

widget_stacks_aruco_stitch::~widget_stacks_aruco_stitch()
{
    delete ui;
}

void widget_stacks_aruco_stitch::on_pushButtonPrev_clicked()
{

}

void widget_stacks_aruco_stitch::on_pushButtonNext_clicked()
{

}

void widget_stacks_aruco_stitch::init_stacked_widget()
{
    dialog_aruco_detector_params_ = new dialog_aruco_detector_params;
    widget_source_selection_ = new widget_source_selection;
    widget_stream_player_ = new widget_stream_player;

    ui->stackedWidget->addWidget(dialog_aruco_detector_params_);
    ui->stackedWidget->addWidget(widget_source_selection_);
    ui->stackedWidget->addWidget(widget_stream_player_);

    ui->stackedWidget->setCurrentWidget(widget_source_selection_);
}

