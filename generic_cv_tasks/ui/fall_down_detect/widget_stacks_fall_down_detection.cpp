#include "widget_stacks_fall_down_detection.hpp"
#include "ui_widget_stacks_fall_down_detection.h"

#include "../obj_detect/widget_object_detect_model_select.hpp"

#include "widget_fall_down_param.hpp"
#include "../widget_roi_selection.hpp"
#include "../widget_source_selection.hpp"
#include "../widget_stream_player.hpp"

#include <multimedia/camera/frame_capture_qcamera.hpp>
#include <multimedia/camera/frame_capture_qcamera_params.hpp>
#include <multimedia/camera/frame_capture_qmediaplayer.hpp>
#include <multimedia/camera/frame_capture_qmediaplayer_params.hpp>
#include <multimedia/camera/frame_process_controller.hpp>
#include <multimedia/network/frame_capture_websocket.hpp>
#include <multimedia/network/frame_capture_websocket_params.hpp>
#include <multimedia/sound/alert_sound_manager.hpp>
#include <multimedia/stream_enum.hpp>

#include <network/websocket_client_controller.hpp>

#include <QMessageBox>
#include <QTimer>

#include <QJsonDocument>
#include <QJsonObject>

using namespace flt;
using namespace flt::mm;

widget_stacks_fall_down_detection::widget_stacks_fall_down_detection(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::widget_stacks_fall_down_detection)
    , msg_box_(new QMessageBox(this))
    , timer_(new QTimer(this))
{
    ui->setupUi(this);

    init_stacked_widget();

    timer_->setInterval(1000);
}

widget_stacks_fall_down_detection::~widget_stacks_fall_down_detection()
{
    delete ui;
}

void widget_stacks_fall_down_detection::on_pushButtonPrev_clicked()
{

}


void widget_stacks_fall_down_detection::on_pushButtonNext_clicked()
{

}

void widget_stacks_fall_down_detection::init_stacked_widget()
{
    widget_fall_down_param_ = new widget_fall_down_param;
    widget_roi_selection_ = new widget_roi_selection;
    widget_object_detect_model_select_ = new widget_object_detect_model_select;
    widget_source_selection_ = new widget_source_selection;
    widget_stream_player_    = new widget_stream_player;

    ui->stackedWidget->addWidget(widget_roi_selection_);
    ui->stackedWidget->addWidget(widget_object_detect_model_select_);
    ui->stackedWidget->addWidget(widget_fall_down_param_);
    ui->stackedWidget->addWidget(widget_source_selection_);
    ui->stackedWidget->addWidget(widget_stream_player_);

    ui->stackedWidget->setCurrentWidget(widget_object_detect_model_select_);
}

