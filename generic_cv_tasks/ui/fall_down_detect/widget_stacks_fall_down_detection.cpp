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

namespace{

inline QString state_widget_fall_down_param(){ return "state_widget_fall_down_param"; }
inline QString state_widget_roi_selection(){ return "state_widget_roi_selection"; }
inline QString state_widget_object_detect_model_select(){ return "state_widget_object_detect_model_select"; }
inline QString state_widget_source_selection(){ return "state_widget_source_selection"; }

inline QString state_version(){ return "state_version"; };

}

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

QJsonObject widget_stacks_fall_down_detection::get_states() const
{
    QJsonObject obj;
    obj[state_widget_fall_down_param()] = widget_fall_down_param_->get_states();
    obj[state_widget_object_detect_model_select()] = widget_object_detect_model_select_->get_states();
    obj[state_widget_roi_selection()] = widget_roi_selection_->get_states();
    obj[state_widget_source_selection()] = widget_source_selection_->get_states();
    obj[state_version()] = "1.0";

    return obj;
}

void widget_stacks_fall_down_detection::set_states(const QJsonObject &val)
{
    if(val.contains(state_widget_fall_down_param())){
        widget_fall_down_param_->set_states(val[state_widget_fall_down_param()].toObject());
    }
    if(val.contains(state_widget_object_detect_model_select())){
        widget_object_detect_model_select_->set_states(val[state_widget_object_detect_model_select()].toObject());
    }
    if(val.contains(state_widget_roi_selection())){
        widget_roi_selection_->set_states(val[state_widget_roi_selection()].toObject());
    }
    if(val.contains(state_widget_source_selection())){
        widget_source_selection_->set_states(val[state_widget_source_selection()].toObject());
    }
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

