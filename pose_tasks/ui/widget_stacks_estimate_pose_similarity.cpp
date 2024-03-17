#include "widget_stacks_estimate_pose_similarity.hpp"
#include "ui_widget_stacks_estimate_pose_similarity.h"

#include "widget_image_pair_player.hpp"
#include "widget_pose_estimation_params.hpp"
#include "widget_source_selection.hpp"

#include <multimedia/camera/frame_process_controller.hpp>
#include <multimedia/network/frame_capture_websocket.hpp>
#include <multimedia/network/frame_capture_websocket_params.hpp>
#include <multimedia/sound/alert_sound_manager.hpp>
#include <multimedia/stream_enum.hpp>

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

