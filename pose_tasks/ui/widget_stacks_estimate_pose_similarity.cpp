#include "widget_stacks_estimate_pose_similarity.hpp"
#include "ui_widget_stacks_estimate_pose_similarity.h"

#include "widget_image_pair_player.hpp"
#include "widget_pose_estimation_params.hpp"
#include "widget_source_selection.hpp"

#include "../algo/estimate_pose_similarity_input.hpp"
#include "../algo/estimate_pose_similarity_worker.hpp"
#include "../algo/estimate_pose_similarity_worker_results.hpp"
#include "../config/config_pose_estimation_worker.hpp"

#include <multimedia/camera/frame_process_controller.hpp>
#include <multimedia/network/frame_capture_websocket.hpp>
#include <multimedia/network/frame_capture_websocket_params.hpp>
#include <multimedia/sound/alert_sound_manager.hpp>
#include <multimedia/stream_enum.hpp>

using namespace flt::mm;

using stype = stream_source_type;

widget_stacks_estimate_pose_similarity::widget_stacks_estimate_pose_similarity(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::widget_stacks_estimate_pose_similarity)
{
    ui->setupUi(this);

    init_stacked_widget();
}

widget_stacks_estimate_pose_similarity::~widget_stacks_estimate_pose_similarity()
{
    delete ui;
}

void widget_stacks_estimate_pose_similarity::init_stacked_widget()
{
    widget_image_pair_player_ = new widget_image_pair_player;
    widget_pose_estimation_params_ = new widget_pose_estimation_params;
    widget_source_selection_ = new widget_source_selection;

    ui->stackedWidget->addWidget(widget_image_pair_player_);
    ui->stackedWidget->addWidget(widget_pose_estimation_params_);
    ui->stackedWidget->addWidget(widget_source_selection_);

    ui->stackedWidget->setCurrentWidget(widget_source_selection_);
}

void widget_stacks_estimate_pose_similarity::next_page_is_estimate_pose_similarity_display()
{
    //ui->stackedWidget->setCurrentWidget(widget_image_pair_player_);

    ui->pushButtonNext->setVisible(false);

    auto config = widget_pose_estimation_params_->get_config();
    config.source_type_ = widget_source_selection_->get_source_type();
    auto worker = new estimate_pose_similarity_worker(config);
    process_controller_ = std::make_shared<frame_process_controller>(worker);

    if(widget_source_selection_->get_source_type() == stype::websocket){
        /*ui->stackedWidget->setCurrentWidget(widget_stream_player_);
        sfwmw_ = std::make_unique<frame_capture_websocket>(widget_source_selection_->get_frame_capture_websocket_params());
        connect(process_controller_.get(), &frame_process_controller::send_process_results,
                this, &widget_stacks_pose_estimation::send_process_results);

        emit process_controller_->start();
        sfwmw_->add_listener(process_controller_, this);
        sfwmw_->start();//*/
    }else{

        ui->stackedWidget->setCurrentWidget(widget_image_pair_player_);
        sfwmw_ = nullptr;
        connect(widget_image_pair_player_, &widget_image_pair_player::source_image_selected,
                this, &widget_stacks_estimate_pose_similarity::process_source_image);
        connect(widget_image_pair_player_, &widget_image_pair_player::target_image_selected,
                this, &widget_stacks_estimate_pose_similarity::process_target_image);
        connect(process_controller_.get(), &frame_process_controller::send_process_results,
                widget_image_pair_player_, &widget_image_pair_player::display_frame);

        emit process_controller_->start();
    }
}

void widget_stacks_estimate_pose_similarity::process_source_image(QImage img)
{
    estimate_pose_similarity_input input;
    input.qimg_ = std::move(img);
    input.is_target_ = false;

    process_controller_->predict(std::move(input));
}

void widget_stacks_estimate_pose_similarity::process_target_image(QImage img)
{
    estimate_pose_similarity_input input;
    input.qimg_ = std::move(img);
    input.is_target_ = true;

    process_controller_->predict(std::move(input));
}

void widget_stacks_estimate_pose_similarity::on_pushButtonPrev_clicked()
{
    ui->pushButtonNext->setVisible(true);
    if(ui->stackedWidget->currentWidget() == widget_pose_estimation_params_){
        ui->stackedWidget->setCurrentWidget(widget_source_selection_);
    }else if(ui->stackedWidget->currentWidget() == widget_image_pair_player_){
        ui->stackedWidget->setCurrentWidget(widget_pose_estimation_params_);
    }else{
        emit prev_button_clicked();
    }
}

void widget_stacks_estimate_pose_similarity::on_pushButtonNext_clicked()
{
    if(ui->stackedWidget->currentWidget() == widget_source_selection_){
        ui->stackedWidget->setCurrentWidget(widget_pose_estimation_params_);
    }else if(ui->stackedWidget->currentWidget() == widget_pose_estimation_params_){
        next_page_is_estimate_pose_similarity_display();
    }
}

