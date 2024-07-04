#include "widget_stacks_action_classify.hpp"
#include "ui_widget_stacks_action_classify.h"

#include "../dialog_alert_sender_settings.hpp"
#include "../frame_capture_creator.hpp"
#include "../widget_roi_selection.hpp"
#include "../widget_source_selection.hpp"
#include "../widget_stream_player.hpp"

#include "widget_action_classify_alert.hpp"
#include "widget_action_classify_model_select.hpp"
#include "widget_select_action_to_classify.hpp"

#include "../../algo/frame_display_worker.hpp"
#include "../../algo/action_classify/pptsm_v2_worker.hpp"
#include "../../config/config_pptsm_v2_worker.hpp"
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

#include <QJsonObject>
#include <QMessageBox>
#include <QTimer>

using namespace flt;
using namespace flt::mm;

namespace{

inline QString state_widget_action_classify_alert(){ return "state_widget_action_classify_alert"; };
inline QString state_widget_action_classify_model_select(){ return "state_widget_action_classify_model_select"; };
inline QString state_widget_roi_selection(){ return "state_widget_roi_selection"; };
inline QString state_widget_select_action_to_classify(){ return "state_widget_select_action_to_classify"; };
inline QString state_widget_source_selection(){ return "state_widget_source_selection"; };

inline QString state_version(){ return "state_version"; };

}

widget_stacks_action_classify::widget_stacks_action_classify(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::widget_stacks_action_classify),
    msg_box_{new QMessageBox(this)}
{
    ui->setupUi(this);

    init_stacked_widget();    
}

widget_stacks_action_classify::~widget_stacks_action_classify()
{
    delete ui;
}

QJsonObject widget_stacks_action_classify::get_states() const
{
    QJsonObject obj;
    obj[state_widget_action_classify_alert()] = widget_action_classify_alert_->get_states();
    obj[state_widget_action_classify_model_select()] = widget_action_classify_model_select_->get_states();
    obj[state_widget_roi_selection()] = widget_roi_selection_->get_states();
    obj[state_widget_select_action_to_classify()] = widget_select_action_to_classify_->get_states();
    obj[state_widget_source_selection()] = widget_source_selection_->get_states();

    obj[state_version()] = "1.0";

    return obj;
}

void widget_stacks_action_classify::set_states(const QJsonObject &val)
{
    if(val.contains(state_widget_action_classify_alert())){
        widget_action_classify_alert_->set_states(val[state_widget_action_classify_alert()].toObject());
    }
    if(val.contains(state_widget_action_classify_model_select())){
        widget_action_classify_model_select_->set_states(val[state_widget_action_classify_model_select()].toObject());
    }
    if(val.contains(state_widget_roi_selection())){
        widget_roi_selection_->set_states(val[state_widget_roi_selection()].toObject());
    }
    if(val.contains(state_widget_select_action_to_classify())){
        widget_select_action_to_classify_->set_states(val[state_widget_select_action_to_classify()].toObject());
    }
    if(val.contains(state_widget_source_selection())){
        widget_source_selection_->set_states(val[state_widget_source_selection()].toObject());
    }
}

void widget_stacks_action_classify::init_stacked_widget()
{
    widget_action_classify_alert_ = new widget_action_classify_alert;
    widget_action_classify_model_select_ = new widget_action_classify_model_select;
    widget_roi_selection_ = new widget_roi_selection;
    widget_select_action_to_classify_ = new widget_select_action_to_classify;
    widget_source_selection_ = new widget_source_selection;
    widget_stream_player_ = new widget_stream_player;

    ui->stackedWidget->addWidget(widget_action_classify_model_select_);    
    ui->stackedWidget->addWidget(widget_select_action_to_classify_);
    ui->stackedWidget->addWidget(widget_action_classify_alert_);
    ui->stackedWidget->addWidget(widget_roi_selection_);
    ui->stackedWidget->addWidget(widget_source_selection_);
    ui->stackedWidget->addWidget(widget_stream_player_);

    fcreator_ = new frame_capture_creator(widget_source_selection_, widget_stream_player_, this);
}

void widget_stacks_action_classify::create_roi_select_stream()
{
    fcreator_->create_frame_capture();
    auto process_controller = std::make_shared<frame_process_controller>(new frame_display_worker);
    connect(process_controller.get(), &frame_process_controller::send_process_results,
            widget_roi_selection_, &widget_roi_selection::display_frame);
    emit process_controller->start();
    fcreator_->get_sfwmw()->add_listener(process_controller, this);
    fcreator_->get_sfwmw()->start();
}

void widget_stacks_action_classify::next_page_is_label_select_roi()
{
    if(!widget_source_selection_->get_is_valid_source()){
        msg_box_->warning(this, tr("Warning"), tr("Invalid url"));
        msg_box_->show();
    }else{
        ui->stackedWidget->setCurrentWidget(widget_roi_selection_);
        ui->pushButtonNext->setEnabled(true);
        ui->pushButtonPrev->setEnabled(true);

        create_roi_select_stream();
    }
}

void widget_stacks_action_classify::next_page_is_widget_stream_player()
{
    ui->stackedWidget->setCurrentWidget(widget_stream_player_);    

    config_ppstm_v2_worker config;
    config.config_action_classify_alert_ = widget_action_classify_alert_->get_config();
    config.config_action_classify_model_select_ = widget_action_classify_model_select_->get_config();
    config.config_select_action_to_classify_ = widget_select_action_to_classify_->get_config();
    config.config_alert_sender_ = get_widget_alert_sender_settings().get_config();
    config.roi_ = widget_roi_selection_->get_norm_rubber_band_rect();

    auto worker = new pptsm_v2_worker(std::move(config));
    connect(&get_widget_alert_sender_settings(), &dialog_alert_sender_settings::button_ok_clicked,
            worker, &pptsm_v2_worker::change_alert_sender_config);
    connect(worker, &pptsm_v2_worker::send_alert_by_binary, this, &widget_stacks_action_classify::send_alert_by_binary);
    connect(worker, &pptsm_v2_worker::send_alert_by_text, this, &widget_stacks_action_classify::send_alert_by_text);

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

void widget_stacks_action_classify::send_alert_by_binary(QByteArray const &msg)
{
    emit get_websocket_controller().send_binary_message(msg);
}

void widget_stacks_action_classify::send_alert_by_text(QString const &msg)
{
    emit get_websocket_controller().send_text_message(msg);
}

void widget_stacks_action_classify::on_pushButtonPrev_clicked()
{
    if(ui->stackedWidget->currentWidget() == widget_select_action_to_classify_){
        ui->stackedWidget->setCurrentWidget(widget_action_classify_model_select_);
    }else if(ui->stackedWidget->currentWidget() == widget_action_classify_alert_){
        ui->stackedWidget->setCurrentWidget(widget_select_action_to_classify_);
    }else if(ui->stackedWidget->currentWidget() == widget_source_selection_){
        ui->stackedWidget->setCurrentWidget(widget_select_action_to_classify_);
    }else if(ui->stackedWidget->currentWidget() == widget_roi_selection_){
        ui->stackedWidget->setCurrentWidget(widget_source_selection_);
        fcreator_->reset();
    }else if(ui->stackedWidget->currentWidget() == widget_stream_player_){
        ui->pushButtonNext->setVisible(true);
        ui->stackedWidget->setCurrentWidget(widget_roi_selection_);
        create_roi_select_stream();        
    }else if(ui->stackedWidget->currentWidget() == widget_action_classify_model_select_){
        emit switch_to_task_selection_page();
    }
}


void widget_stacks_action_classify::on_pushButtonNext_clicked()
{
    if(ui->stackedWidget->currentWidget() == widget_action_classify_model_select_){
        ui->stackedWidget->setCurrentWidget(widget_select_action_to_classify_);
    }else if(ui->stackedWidget->currentWidget() == widget_select_action_to_classify_){
        ui->stackedWidget->setCurrentWidget(widget_action_classify_alert_);
    }else if(ui->stackedWidget->currentWidget() == widget_action_classify_alert_){
        ui->stackedWidget->setCurrentWidget(widget_source_selection_);
    }else if(ui->stackedWidget->currentWidget() == widget_source_selection_){
        next_page_is_label_select_roi();
    }else if(ui->stackedWidget->currentWidget() == widget_roi_selection_){
        ui->pushButtonNext->setVisible(false);
        next_page_is_widget_stream_player();
    }
}

