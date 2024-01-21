#include "widget_stacks_aruco_detector.hpp"
#include "ui_widget_stacks_aruco_detector.h"

#include "../algo/aruco_detect_worker.hpp"

#include "../config/config_aruco_detect_worker.hpp"

#include "dialog_aruco_detector_params.hpp"
#include "widget_image_player.hpp"
#include "widget_stream_player.hpp"
#include "widget_source_selection.hpp"

#include <multimedia/camera/frame_process_controller.hpp>
#include <multimedia/network/frame_capture_websocket.hpp>
#include <multimedia/network/frame_capture_websocket_params.hpp>
#include <multimedia/sound/alert_sound_manager.hpp>
#include <multimedia/stream_enum.hpp>

using namespace flt::mm;

widget_stacks_aruco_detector::widget_stacks_aruco_detector(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::widget_stacks_aruco_detector)
{
    ui->setupUi(this);

    init_stacked_widget();
}

widget_stacks_aruco_detector::~widget_stacks_aruco_detector()
{
    delete ui;
}

void widget_stacks_aruco_detector::init_stacked_widget()
{
    dialog_aruco_detector_params_ = new dialog_aruco_detector_params;
    widget_image_player_ = new widget_image_player;
    widget_source_selection_ = new widget_source_selection;
    widget_stream_player_ = new widget_stream_player;

    ui->stackedWidget->addWidget(dialog_aruco_detector_params_);
    ui->stackedWidget->addWidget(widget_source_selection_);
    ui->stackedWidget->addWidget(widget_image_player_);
    ui->stackedWidget->addWidget(widget_stream_player_);

    ui->stackedWidget->setCurrentWidget(widget_source_selection_);
}

void widget_stacks_aruco_detector::display_frame(std::any input)
{
    widget_image_player_->display_frame(std::move(input));
}

void widget_stacks_aruco_detector::next_page_is_widget_image_player()
{
    ui->pushButtonNext->setVisible(false);

    config_aruco_detect_worker config = dialog_aruco_detector_params_->get_config();
    auto worker = new aruco_detect_worker(std::move(config));

    if(widget_source_selection_->get_source_type() == stream_source_type::websocket){
        ui->stackedWidget->setCurrentWidget(widget_stream_player_);
        sfwmw_ = std::make_unique<frame_capture_websocket>(widget_source_selection_->get_frame_capture_websocket_params());
        process_controller_ = std::make_shared<frame_process_controller>(worker);
        connect(process_controller_.get(), &frame_process_controller::send_process_results,
                widget_stream_player_, &widget_stream_player::display_frame);

        emit process_controller_->start();
        sfwmw_->add_listener(process_controller_, this);
        sfwmw_->start();
    }else{
        ui->stackedWidget->setCurrentWidget(widget_image_player_);
        sfwmw_ = nullptr;
        process_controller_ = std::make_shared<frame_process_controller>(worker);
        connect(widget_image_player_, &widget_image_player::image_selected,
                process_controller_.get(), &frame_process_controller::predict);
        connect(process_controller_.get(), &frame_process_controller::send_process_results,
                this, &widget_stacks_aruco_detector::display_frame);

        emit process_controller_->start();

        widget_image_player_->call_image_selected();
    }
}

void widget_stacks_aruco_detector::on_pushButtonPrev_clicked()
{
    ui->pushButtonNext->setVisible(true);
    if(ui->stackedWidget->currentWidget() == widget_image_player_){
        ui->stackedWidget->setCurrentWidget(dialog_aruco_detector_params_);
    }else if(ui->stackedWidget->currentWidget() == dialog_aruco_detector_params_){
        ui->stackedWidget->setCurrentWidget(widget_source_selection_);
    }else{
        emit prev_button_clicked();
    }
}


void widget_stacks_aruco_detector::on_pushButtonNext_clicked()
{
    ui->pushButtonPrev->setVisible(true);

    if(ui->stackedWidget->currentWidget() == widget_source_selection_){
        ui->stackedWidget->setCurrentWidget(dialog_aruco_detector_params_);
    }else if(ui->stackedWidget->currentWidget() == dialog_aruco_detector_params_){
        ui->pushButtonNext->setVisible(false);
        next_page_is_widget_image_player();
    }
}

