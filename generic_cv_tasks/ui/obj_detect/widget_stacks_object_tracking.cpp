#include "widget_stacks_object_tracking.hpp"
#include "ui_widget_stacks_object_tracking.h"

#include "../dialog_alert_sender_settings.hpp"

#include "../frame_capture_creator.hpp"

#include "../widget_roi_selection.hpp"
#include "../widget_source_selection.hpp"
#include "../widget_stream_player.hpp"

#include "widget_object_detect_model_select.hpp"
#include "widget_select_object_to_detect.hpp"
#include "widget_tracker_alert.hpp"

#include "../../algo/obj_detect/nanodet_worker.hpp"

#include "../../config/config_alert_sender.hpp"
#include "../../config/config_nanodet_worker.hpp"

#include "../../global/global_keywords.hpp"
#include "../../global/global_object.hpp"

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

inline QString state_version(){ return "state_version"; };

}

widget_stacks_object_tracking::widget_stacks_object_tracking(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::widget_stacks_object_tracking)
    , msg_box_{new QMessageBox(this)}
{
    ui->setupUi(this);

    init_stacked_widget();
}

widget_stacks_object_tracking::~widget_stacks_object_tracking()
{
    delete ui;
}

QJsonObject widget_stacks_object_tracking::get_states() const
{
    QJsonObject obj;
    global_keywords const gk;
    obj[gk.state_roi()] = widget_roi_selection_->get_states();
    obj[gk.state_widget_object_detect_model_select()] = widget_object_detect_model_select_->get_states();
    obj[gk.state_widget_select_object_to_detect()] = widget_select_object_to_detect_->get_states();
    obj[gk.state_widget_source_selection()] = widget_source_selection_->get_states();
    obj[gk.state_tracker_alert()] = widget_tracker_alert_->get_states();

    obj[state_version()] = "1.0";

    return obj;
}

void widget_stacks_object_tracking::set_states(const QJsonObject &val)
{
    global_keywords const gk;
    if(val.contains(gk.state_roi())){
        widget_roi_selection_->set_states(val[gk.state_roi()].toObject());
    }
    if(val.contains(gk.state_widget_object_detect_model_select())){
        widget_object_detect_model_select_->set_states(val[gk.state_widget_object_detect_model_select()].toObject());
    }
    if(val.contains(gk.state_widget_select_object_to_detect())){
        widget_select_object_to_detect_->set_states(val[gk.state_widget_select_object_to_detect()].toObject());
    }
    if(val.contains(gk.state_widget_source_selection())){
        widget_source_selection_->set_states(val[gk.state_widget_source_selection()].toObject());
    }
    if(val.contains(gk.state_tracker_alert())){
        widget_tracker_alert_->set_states(val[gk.state_tracker_alert()].toObject());
    }
}

void widget_stacks_object_tracking::init_stacked_widget()
{
    widget_roi_selection_ = new widget_roi_selection;
    widget_object_detect_model_select_ = new widget_object_detect_model_select;
    widget_source_selection_ = new widget_source_selection;
    widget_stream_player_    = new widget_stream_player;
    widget_tracker_alert_    = new widget_tracker_alert;

    global_keywords const gk;
    widget_select_object_to_detect_ = new widget_select_object_to_detect(gk.coco_names());

    ui->stackedWidget->addWidget(widget_roi_selection_);
    ui->stackedWidget->addWidget(widget_object_detect_model_select_);
    ui->stackedWidget->addWidget(widget_select_object_to_detect_);
    ui->stackedWidget->addWidget(widget_source_selection_);
    ui->stackedWidget->addWidget(widget_stream_player_);
    ui->stackedWidget->addWidget(widget_tracker_alert_);

    ui->stackedWidget->setCurrentWidget(widget_object_detect_model_select_);

    fcreator_ = new frame_capture_creator(widget_source_selection_, widget_stream_player_, this);

    if(!more_than_one_task()){
        ui->pushButtonPrev->setVisible(false);
    }
}

void widget_stacks_object_tracking::create_roi_select_stream()
{
    fcreator_->create_roi_select_stream(widget_roi_selection_);
}

void widget_stacks_object_tracking::next_page_is_label_select_roi()
{
    if(!widget_source_selection_->get_is_valid_source()){
        msg_box_->warning(this, tr("Warning"), tr("Invalid url"));        
    }else{        
        ui->stackedWidget->setCurrentWidget(widget_roi_selection_);        
        create_roi_select_stream();
    }
}

void widget_stacks_object_tracking::next_page_is_widget_stream_player()
{    
    ui->stackedWidget->setCurrentWidget(widget_stream_player_);

    config_nanodet_worker config;
    config.config_alert_sender_ = get_widget_alert_sender_settings().get_config();
    config.config_object_detect_model_select_ = widget_object_detect_model_select_->get_config();
    config.config_select_object_to_detect_ = widget_select_object_to_detect_->get_config();
    config.roi_ = widget_roi_selection_->get_norm_rubber_band_rect();
    config.config_tracker_alert_ = widget_tracker_alert_->get_config();
    config.source_type_ = widget_source_selection_->get_source_type();

    auto worker = new nanodet_worker(std::move(config));
    connect(&get_widget_alert_sender_settings(), &dialog_alert_sender_settings::button_ok_clicked,
            worker, &nanodet_worker::change_alert_sender_config);
    connect(worker, &nanodet_worker::send_alert_by_binary, this, &widget_stacks_object_tracking::send_alert_by_binary);
    connect(worker, &nanodet_worker::send_alert_by_text, this, &widget_stacks_object_tracking::send_alert_by_text);

    auto process_controller = std::make_shared<frame_process_controller>(worker);
    connect(process_controller.get(), &frame_process_controller::send_process_results,
            widget_stream_player_, &widget_stream_player::display_frame);

    if(get_widget_alert_sender_settings().get_config().activate_){
        emit get_websocket_controller().reopen_if_needed(get_widget_alert_sender_settings().get_config().url_);
    }

    fcreator_->create_frame_capture();
    emit process_controller->start();
    fcreator_->get_sfwmw()->add_listener(process_controller, this);
    fcreator_->get_sfwmw()->start();
}

void widget_stacks_object_tracking::next_page_is_widget_select_object_to_detect()
{    
    ui->stackedWidget->setCurrentWidget(widget_select_object_to_detect_);    
}

void widget_stacks_object_tracking::next_page_is_widget_source_selection()
{    
    ui->stackedWidget->setCurrentWidget(widget_source_selection_);    
}

void widget_stacks_object_tracking::next_page_is_widget_tracker_alert()
{    
    ui->stackedWidget->setCurrentWidget(widget_tracker_alert_);    
}

void widget_stacks_object_tracking::send_alert_by_binary(const QByteArray &msg)
{
    emit get_websocket_controller().send_binary_message(msg);
}

void widget_stacks_object_tracking::send_alert_by_text(const QString &msg)
{
    emit get_websocket_controller().send_text_message(msg);
}

void widget_stacks_object_tracking::on_pushButtonPrev_clicked()
{    
    fcreator_->reset();
    ui->pushButtonNext->setVisible(true);
    ui->pushButtonPrev->setVisible(true);

    if(ui->stackedWidget->currentWidget() == widget_select_object_to_detect_){
        if(!more_than_one_task()){
            ui->pushButtonPrev->setVisible(false);
        }
        ui->stackedWidget->setCurrentWidget(widget_object_detect_model_select_);
    }else if(ui->stackedWidget->currentWidget() == widget_source_selection_){
        ui->stackedWidget->setCurrentWidget(widget_tracker_alert_);
    }else if(ui->stackedWidget->currentWidget() == widget_tracker_alert_){
        ui->stackedWidget->setCurrentWidget(widget_select_object_to_detect_);        
    }else if(ui->stackedWidget->currentWidget() == widget_roi_selection_){
        ui->stackedWidget->setCurrentWidget(widget_source_selection_);        
    }else if(ui->stackedWidget->currentWidget() == widget_stream_player_){
        ui->stackedWidget->setCurrentWidget(widget_roi_selection_);
        create_roi_select_stream();
    }else if(ui->stackedWidget->currentWidget() == widget_object_detect_model_select_){
        if(more_than_one_task()){
            emit switch_to_task_selection_page();
        }
    }
}

void widget_stacks_object_tracking::on_pushButtonNext_clicked()
{
    ui->pushButtonPrev->setVisible(true);
    if(ui->stackedWidget->currentWidget() == widget_object_detect_model_select_){
        get_alert_sound_manager();
        next_page_is_widget_select_object_to_detect();
    }else if(ui->stackedWidget->currentWidget() == widget_select_object_to_detect_){
        next_page_is_widget_tracker_alert();
    }else if(ui->stackedWidget->currentWidget() == widget_tracker_alert_){
        next_page_is_widget_source_selection();
    }else if(ui->stackedWidget->currentWidget() == widget_source_selection_){
        next_page_is_label_select_roi();
    }else if(ui->stackedWidget->currentWidget() == widget_roi_selection_){
        next_page_is_widget_stream_player();

        ui->pushButtonNext->setVisible(false);
    }
}

