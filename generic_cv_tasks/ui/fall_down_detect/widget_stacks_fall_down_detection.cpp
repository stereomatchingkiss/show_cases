#include "widget_stacks_fall_down_detection.hpp"
#include "ui_widget_stacks_fall_down_detection.h"

#include "../obj_detect/widget_object_detect_model_select.hpp"
#include "../../global/global_keywords.hpp"

#include "../frame_capture_creator.hpp"
#include "../widget_roi_selection.hpp"
#include "../widget_source_selection.hpp"
#include "../widget_stream_player.hpp"

#include "widget_fall_down_param.hpp"
#include "widget_fall_down_obj_det_alert.hpp"

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

}

widget_stacks_fall_down_detection::widget_stacks_fall_down_detection(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::widget_stacks_fall_down_detection)
    , msg_box_{new QMessageBox(this)}
{
    ui->setupUi(this);

    init_stacked_widget();    
}

widget_stacks_fall_down_detection::~widget_stacks_fall_down_detection()
{
    delete ui;
}

QJsonObject widget_stacks_fall_down_detection::get_states() const
{
    QJsonObject obj;
    global_keywords const gk;
    obj[state_widget_fall_down_param()] = widget_fall_down_param_->get_states();
    obj[gk.state_widget_object_detect_model_select()] = widget_object_detect_model_select_->get_states();
    obj[gk.state_roi()] = widget_roi_selection_->get_states();
    obj[gk.state_widget_source_selection()] = widget_source_selection_->get_states();

    obj[gk.state_version()] = "1.0";

    return obj;
}

void widget_stacks_fall_down_detection::set_states(const QJsonObject &val)
{
    global_keywords const gk;

    if(val.contains(state_widget_fall_down_param())){
        widget_fall_down_param_->set_states(val[state_widget_fall_down_param()].toObject());
    }
    if(val.contains(gk.state_widget_object_detect_model_select())){
        widget_object_detect_model_select_->set_states(val[gk.state_widget_object_detect_model_select()].toObject());
    }
    if(val.contains(gk.state_roi())){
        widget_roi_selection_->set_states(val[gk.state_roi()].toObject());
    }
    if(val.contains(gk.state_widget_source_selection())){
        widget_source_selection_->set_states(val[gk.state_widget_source_selection()].toObject());
    }
}

void widget_stacks_fall_down_detection::on_pushButtonPrev_clicked()
{

}

void widget_stacks_fall_down_detection::on_pushButtonNext_clicked()
{
    ui->pushButtonNext->setVisible(true);

    if(ui->stackedWidget->currentWidget() == widget_object_detect_model_select_){
        ui->stackedWidget->setCurrentWidget(widget_fall_down_param_);
    }else if(ui->stackedWidget->currentWidget() == widget_fall_down_param_){
        ui->stackedWidget->setCurrentWidget(widget_fall_down_obj_det_alert_);
    }else if(ui->stackedWidget->currentWidget() == widget_fall_down_obj_det_alert_){
        ui->stackedWidget->setCurrentWidget(widget_source_selection_);
    }else if(ui->stackedWidget->currentWidget() == widget_source_selection_){
        if(!widget_source_selection_->get_is_valid_source()){
            msg_box_->warning(this, tr("Warning"), tr("Invalid url"));
            msg_box_->show();
        }else{
            ui->stackedWidget->setCurrentWidget(widget_roi_selection_);
            fcreator_->create_roi_select_stream(widget_roi_selection_);
        }
    }else if(ui->stackedWidget->currentWidget() == widget_roi_selection_){
        ui->stackedWidget->setCurrentWidget(widget_stream_player_);
    }else if(ui->stackedWidget->currentWidget() == widget_stream_player_){
        ui->pushButtonNext->setVisible(false);
    }
}

void widget_stacks_fall_down_detection::init_stacked_widget()
{
    widget_fall_down_param_ = new widget_fall_down_param;
    widget_fall_down_obj_det_alert_ = new widget_fall_down_obj_det_alert;
    widget_roi_selection_ = new widget_roi_selection;
    widget_object_detect_model_select_ = new widget_object_detect_model_select;    
    widget_source_selection_ = new widget_source_selection;
    widget_stream_player_    = new widget_stream_player;

    ui->stackedWidget->addWidget(widget_roi_selection_);
    ui->stackedWidget->addWidget(widget_object_detect_model_select_);    
    ui->stackedWidget->addWidget(widget_fall_down_param_);
    ui->stackedWidget->addWidget(widget_fall_down_obj_det_alert_);
    ui->stackedWidget->addWidget(widget_source_selection_);
    ui->stackedWidget->addWidget(widget_stream_player_);

    ui->stackedWidget->setCurrentWidget(widget_object_detect_model_select_);

    fcreator_ = new frame_capture_creator(widget_source_selection_, widget_stream_player_, this);
}

void widget_stacks_fall_down_detection::next_page_is_widget_stream_player()
{
    ui->stackedWidget->setCurrentWidget(widget_stream_player_);

    /*config_nanodet_worker config;
    config.config_alert_sender_ = get_widget_alert_sender_settings().get_config();
    config.config_object_detect_model_select_ = widget_object_detect_model_select_->get_config();
    config.config_select_object_to_detect_ = widget_select_object_to_detect_->get_config();
    config.roi_ = widget_roi_selection_->get_norm_rubber_band_rect();
    config.config_tracker_alert_ = widget_tracker_alert_->get_config();

    auto worker = new nanodet_worker(std::move(config));
    connect(&get_widget_alert_sender_settings(), &dialog_alert_sender_settings::button_ok_clicked,
            worker, &nanodet_worker::change_alert_sender_config);
    connect(worker, &nanodet_worker::send_alert_by_binary, this, &widget_stacks_object_tracking::send_alert_by_binary);
    connect(worker, &nanodet_worker::send_alert_by_text, this, &widget_stacks_object_tracking::send_alert_by_text);

    auto process_controller = std::make_shared<frame_process_controller>(worker);
    connect(process_controller.get(), &frame_process_controller::send_process_results,
            widget_stream_player_, &widget_stream_player::display_frame);//*/

    //if(get_widget_alert_sender_settings().get_config().activate_){
    //    emit get_websocket_controller().reopen_if_needed(get_widget_alert_sender_settings().get_config().url_);
    //}

    /*fcreator_->create_frame_capture();
    emit process_controller->start();
    fcreator_->get_sfwmw()->add_listener(process_controller, this);
    fcreator_->get_sfwmw()->start();//*/
}
