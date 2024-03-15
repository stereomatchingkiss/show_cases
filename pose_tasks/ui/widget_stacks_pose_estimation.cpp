#include "widget_stacks_pose_estimation.hpp"
#include "ui_widget_stacks_pose_estimation.h"

#include "widget_image_player.hpp"
#include "widget_pose_estimation_params.hpp"
#include "widget_source_selection.hpp"
#include "widget_stream_player.hpp"

#include "../algo/pose_estimation_worker.hpp"
#include "../config/config_pose_estimation_worker.hpp"

#include <multimedia/camera/frame_process_controller.hpp>
#include <multimedia/network/frame_capture_websocket.hpp>
#include <multimedia/network/frame_capture_websocket_params.hpp>
#include <multimedia/sound/alert_sound_manager.hpp>
#include <multimedia/stream_enum.hpp>

using namespace flt::mm;

using stype = stream_source_type;

widget_stacks_pose_estimation::widget_stacks_pose_estimation(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::widget_stacks_pose_estimation)
{
    ui->setupUi(this);

    init_stacked_widget();
}

widget_stacks_pose_estimation::~widget_stacks_pose_estimation()
{
    delete ui;
}

void widget_stacks_pose_estimation::on_pushButtonPrev_clicked()
{
    ui->pushButtonNext->setVisible(true);
    if(ui->stackedWidget->currentWidget() == widget_pose_estimation_params_){
        ui->stackedWidget->setCurrentWidget(widget_source_selection_);
        ui->pushButtonPrev->setVisible(false);
    }else if(ui->stackedWidget->currentWidget() == widget_image_player_ || ui->stackedWidget->currentWidget() == widget_stream_player_){
        ui->stackedWidget->setCurrentWidget(widget_pose_estimation_params_);
    }
}

void widget_stacks_pose_estimation::on_pushButtonNext_clicked()
{
    ui->pushButtonPrev->setVisible(true);
    if(ui->stackedWidget->currentWidget() == widget_source_selection_){
        ui->stackedWidget->setCurrentWidget(widget_pose_estimation_params_);
    }else if(ui->stackedWidget->currentWidget() == widget_pose_estimation_params_){
        next_page_is_pose_estimation_display();
    }
}

void widget_stacks_pose_estimation::init_stacked_widget()
{
    widget_pose_estimation_params_ = new widget_pose_estimation_params;
    widget_image_player_ = new widget_image_player;
    widget_source_selection_ = new widget_source_selection;
    widget_stream_player_ = new widget_stream_player;

    ui->stackedWidget->addWidget(widget_pose_estimation_params_);
    ui->stackedWidget->addWidget(widget_source_selection_);
    ui->stackedWidget->addWidget(widget_image_player_);
    ui->stackedWidget->addWidget(widget_stream_player_);

    ui->stackedWidget->setCurrentWidget(widget_source_selection_);
}

void widget_stacks_pose_estimation::next_page_is_pose_estimation_display()
{
    ui->pushButtonNext->setVisible(false);

    auto config = widget_pose_estimation_params_->get_config();
    config.source_type_ = widget_source_selection_->get_source_type();
    auto worker = new pose_estimation_worker(config);

    if(widget_source_selection_->get_source_type() == stype::websocket){
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
                widget_image_player_, &widget_image_player::display_frame);

        emit process_controller_->start();

        widget_image_player_->call_image_selected();
    }
}

