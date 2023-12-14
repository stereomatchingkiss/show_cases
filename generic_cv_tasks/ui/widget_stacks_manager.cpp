#include "widget_stacks_manager.hpp"
#include "ui_widget_stacks_manager.h"

#include "widget_alert_sender_settings.hpp"
#include "widget_object_detect_model_select.hpp"
#include "widget_select_object_to_detect.hpp"
#include "widget_source_selection.hpp"
#include "widget_stream_player.hpp"
#include "widget_tracker_alert.hpp"

#include "../algo/frame_display_worker.hpp"
#include "../algo/obj_detect/nanodet_worker.hpp"

#include "../config/config_alert_sender.hpp"
#include "../config/config_nanodet_worker.hpp"
#include "../config/config_read_write.hpp"

#include "../global/global_keywords.hpp"
#include "../global/global_object.hpp"

#include <ui/label_select_roi.hpp>

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

#include <QFileDialog>
#include <QMessageBox>
#include <QTimer>

#include <QJsonDocument>
#include <QJsonObject>

using namespace flt;
using namespace flt::mm;

widget_stacks_manager::widget_stacks_manager(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::widget_stacks_manager)    
    , msg_box_(new QMessageBox(this))
    , timer_(new QTimer(this))
{
    ui->setupUi(this);

    init_stacked_widget();

    ui->pushButtonPrev->setEnabled(false);
    ui->labelTitle->setText(tr("Select model"));

    setMinimumSize(QSize(600, 400));

    timer_->setInterval(1000);
}

widget_stacks_manager::~widget_stacks_manager()
{
    delete ui;
}

QJsonObject widget_stacks_manager::get_states() const
{
    QJsonObject obj;
    global_keywords const gk;
    obj[gk.state_roi()] = label_select_roi_->get_states();
    obj[gk.state_widget_object_detect_model_select()] = widget_object_detect_model_select_->get_states();
    obj[gk.state_widget_select_object_to_detect()] = widget_select_object_to_detect_->get_states();
    obj[gk.state_widget_source_selection()] = widget_source_selection_->get_states();
    obj[gk.state_tracker_alert()] = widget_tracker_alert_->get_states();

    return obj;
}

void widget_stacks_manager::set_states(const QJsonObject &val)
{
    global_keywords const gk;
    if(val.contains(gk.state_roi())){
        label_select_roi_->set_states(val[gk.state_roi()].toObject());
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

void widget_stacks_manager::on_pushButtonPrev_clicked()
{
    sfwmw_.reset();
    ui->labelTitle->setVisible(true);
    if(ui->stackedWidget->currentWidget() == widget_select_object_to_detect_){
        ui->labelTitle->setText(tr("Select model"));
        ui->stackedWidget->setCurrentWidget(widget_object_detect_model_select_);
        ui->pushButtonNext->setEnabled(true);
        ui->pushButtonPrev->setEnabled(false);
    }else if(ui->stackedWidget->currentWidget() == widget_source_selection_){
        ui->labelTitle->setText(tr("Alert settings"));
        ui->stackedWidget->setCurrentWidget(widget_tracker_alert_);
        ui->pushButtonNext->setEnabled(true);
        ui->pushButtonPrev->setEnabled(true);
    }else if(ui->stackedWidget->currentWidget() == widget_tracker_alert_){
        ui->labelTitle->setText(tr("Select objects to detect"));
        ui->stackedWidget->setCurrentWidget(widget_select_object_to_detect_);
        ui->pushButtonNext->setEnabled(true);
        ui->pushButtonPrev->setEnabled(true);
    }else if(ui->stackedWidget->currentWidget() == label_select_roi_){
        ui->labelTitle->setText(tr("Select stream"));
        ui->stackedWidget->setCurrentWidget(widget_source_selection_);
        ui->pushButtonNext->setEnabled(true);
        ui->pushButtonPrev->setEnabled(true);
    }else if(ui->stackedWidget->currentWidget() == widget_stream_player_){
        ui->labelTitle->setText(tr("Select roi"));
        ui->stackedWidget->setCurrentWidget(label_select_roi_);
        ui->pushButtonNext->setEnabled(true);
        ui->pushButtonPrev->setEnabled(true);

        create_roi_select_stream();
    }
}

void widget_stacks_manager::on_pushButtonNext_clicked()
{
    if(ui->stackedWidget->currentWidget() == widget_object_detect_model_select_){
        get_alert_sound_manager();
        next_page_is_widget_select_object_to_detect();
    }else if(ui->stackedWidget->currentWidget() == widget_select_object_to_detect_){
        next_page_is_widget_tracker_alert();
    }else if(ui->stackedWidget->currentWidget() == widget_tracker_alert_){
        next_page_is_widget_source_selection();
    }else if(ui->stackedWidget->currentWidget() == widget_source_selection_){
        next_page_is_label_select_roi();
    }else if(ui->stackedWidget->currentWidget() == label_select_roi_){
        next_page_is_widget_stream_player();
    }
}

void widget_stacks_manager::init_stacked_widget()
{
    label_select_roi_ = new ui::label_select_roi(tr("Select roi"));
    widget_object_detect_model_select_ = new widget_object_detect_model_select;
    widget_source_selection_ = new widget_source_selection;
    widget_stream_player_    = new widget_stream_player;
    widget_tracker_alert_    = new widget_tracker_alert;

    global_keywords gk;
    widget_select_object_to_detect_ = new widget_select_object_to_detect(gk.coco_names());

    ui->stackedWidget->addWidget(label_select_roi_);
    ui->stackedWidget->addWidget(widget_object_detect_model_select_);
    ui->stackedWidget->addWidget(widget_select_object_to_detect_);
    ui->stackedWidget->addWidget(widget_source_selection_);
    ui->stackedWidget->addWidget(widget_stream_player_);
    ui->stackedWidget->addWidget(widget_tracker_alert_);

    ui->stackedWidget->setCurrentWidget(widget_object_detect_model_select_);
}

void widget_stacks_manager::create_frame_capture()
{
    timer_->stop();
    disconnect(timer_, &QTimer::timeout, this, &widget_stacks_manager::update_position);
    if(widget_source_selection_->get_source_type() == stream_source_type::websocket){
        sfwmw_ = std::make_unique<frame_capture_websocket>(widget_source_selection_->get_frame_capture_websocket_params());
    }else if(widget_source_selection_->get_source_type() == stream_source_type::webcam){
        sfwmw_ = std::make_unique<frame_capture_qcamera>(widget_source_selection_->get_frame_capture_qcamera_params());
    }else{
        sfwmw_ = std::make_unique<frame_capture_qmediaplayer>(widget_source_selection_->get_frame_capture_qmediaplayer_params());
        auto player = static_cast<frame_capture_qmediaplayer*>(sfwmw_.get());

        connect(widget_stream_player_, &widget_stream_player::pause, player, &frame_capture_qmediaplayer::pause);
        connect(widget_stream_player_, &widget_stream_player::play, player, &frame_capture_qmediaplayer::start);
        connect(widget_stream_player_, &widget_stream_player::seek, player, &frame_capture_qmediaplayer::set_position);

        connect(timer_, &QTimer::timeout, this, &widget_stacks_manager::update_position);
    }
}

void widget_stacks_manager::create_roi_select_stream()
{
    create_frame_capture();
    auto process_controller = std::make_shared<frame_process_controller>(new frame_display_worker);
    connect(process_controller.get(), &frame_process_controller::send_process_results,
            label_select_roi_, &ui::label_select_roi::display_frame);
    emit process_controller->start();
    sfwmw_->add_listener(process_controller, this);
    sfwmw_->start();
}

void widget_stacks_manager::next_page_is_label_select_roi()
{
    if(!widget_source_selection_->get_is_valid_source()){
        msg_box_->warning(this, tr("Warning"), tr("Invalid url"));
        msg_box_->show();
    }else{
        ui->labelTitle->setText(tr("Select roi"));
        ui->stackedWidget->setCurrentWidget(label_select_roi_);
        ui->pushButtonNext->setEnabled(true);
        ui->pushButtonPrev->setEnabled(true);

        create_roi_select_stream();
    }
}

void widget_stacks_manager::next_page_is_widget_stream_player()
{
    ui->labelTitle->setVisible(false);
    ui->stackedWidget->setCurrentWidget(widget_stream_player_);
    ui->pushButtonNext->setEnabled(false);
    ui->pushButtonPrev->setEnabled(true);

    config_nanodet_worker config;
    config.config_alert_sender_ = get_widget_alert_sender_settings().get_config();
    config.config_object_detect_model_select_ = widget_object_detect_model_select_->get_config();
    config.config_select_object_to_detect_ = widget_select_object_to_detect_->get_config();
    config.roi_ = label_select_roi_->get_norm_rubber_band_rect();
    config.config_tracker_alert_ = widget_tracker_alert_->get_config();

    auto worker = new nanodet_worker(std::move(config));
    connect(&get_widget_alert_sender_settings(), &widget_alert_sender_settings::button_ok_clicked,
            worker, &nanodet_worker::change_alert_sender_config);
    connect(worker, &nanodet_worker::send_alert_by_binary, this, &widget_stacks_manager::send_alert_by_binary);
    connect(worker, &nanodet_worker::send_alert_by_text, this, &widget_stacks_manager::send_alert_by_text);

    auto process_controller = std::make_shared<frame_process_controller>(worker);
    connect(process_controller.get(), &frame_process_controller::send_process_results,
            widget_stream_player_, &widget_stream_player::display_frame);

    if(get_widget_alert_sender_settings().get_config().activate_){
        emit get_websocket_controller().reopen_if_needed(get_widget_alert_sender_settings().get_config().url_);
    }
    create_frame_capture();
    emit process_controller->start();
    sfwmw_->add_listener(process_controller, this);
    sfwmw_->start();

    if(widget_source_selection_->get_source_type() == stream_source_type::hls ||
        widget_source_selection_->get_source_type() == stream_source_type::video){
        auto player = static_cast<frame_capture_qmediaplayer*>(sfwmw_.get());
        widget_stream_player_->set_is_seekable(player->is_seekable());
        widget_stream_player_->set_duration(player->position(), player->max_position());
        connect(timer_, &QTimer::timeout, this, &widget_stacks_manager::update_position);
        timer_->start();
    }else{
        widget_stream_player_->set_is_seekable(false);
    }
}

void widget_stacks_manager::next_page_is_widget_select_object_to_detect()
{
    ui->labelTitle->setText(tr("Select objects to detect"));
    ui->stackedWidget->setCurrentWidget(widget_select_object_to_detect_);
    ui->pushButtonNext->setEnabled(true);
    ui->pushButtonPrev->setEnabled(true);
}

void widget_stacks_manager::next_page_is_widget_source_selection()
{
    ui->labelTitle->setText(tr("Select steram"));
    ui->stackedWidget->setCurrentWidget(widget_source_selection_);
    ui->pushButtonNext->setEnabled(true);
    ui->pushButtonPrev->setEnabled(true);
}

void widget_stacks_manager::next_page_is_widget_tracker_alert()
{
    ui->labelTitle->setText(tr("Alert settings"));
    ui->stackedWidget->setCurrentWidget(widget_tracker_alert_);
    ui->pushButtonNext->setEnabled(true);
    ui->pushButtonPrev->setEnabled(true);
}

void widget_stacks_manager::send_alert_by_binary(const QByteArray &msg)
{
    emit get_websocket_controller().send_binary_message(msg);
}

void widget_stacks_manager::send_alert_by_text(const QString &msg)
{
    emit get_websocket_controller().send_text_message(msg);
}

void widget_stacks_manager::update_position()
{
    if(sfwmw_){
        auto player = static_cast<frame_capture_qmediaplayer*>(sfwmw_.get());
        widget_stream_player_->set_current_position(player->position());
    }
}

