#include "widget_stacks_anpr.hpp"
#include "ui_widget_stacks_anpr.h"

#include "../algo/anpr_worker.hpp"

#include "../config/config_anpr_worker.hpp"

#include "widget_object_detect_model_select.hpp"
#include "widget_stream_player.hpp"

#include <multimedia/camera/frame_process_controller.hpp>
#include <multimedia/camera/single_frame_with_multi_worker_base.hpp>
#include <multimedia/network/frame_capture_websocket.hpp>
#include <multimedia/network/frame_capture_websocket_params.hpp>
#include <multimedia/stream_enum.hpp>

using stype = flt::mm::stream_source_type;

using namespace flt::mm;

widget_stacks_anpr::widget_stacks_anpr(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::widget_stacks_anpr)
{
    ui->setupUi(this);

    init_stacked_widget();
}

widget_stacks_anpr::~widget_stacks_anpr()
{
    delete ui;
}

void widget_stacks_anpr::on_pushButtonPrev_clicked()
{
    ui->pushButtonNext->setVisible(true);

    if(ui->stackedWidget->currentWidget() == widget_stream_player_){
        ui->stackedWidget->setCurrentWidget(widget_object_detect_model_select_);

        sfwmw_ = nullptr;
        process_controller_ = nullptr;
    }
}

void widget_stacks_anpr::on_pushButtonNext_clicked()
{
    ui->pushButtonPrev->setVisible(true);

    if(ui->stackedWidget->currentWidget() == widget_object_detect_model_select_){
        ui->stackedWidget->setCurrentWidget(widget_stream_player_);

        widget_stream_player_->set_source_type(stype::image);
        config_anpr_worker worker_config;
        worker_config.config_object_detect_model_select_ = widget_object_detect_model_select_->get_config();

        sfwmw_ = nullptr;
        process_controller_ = std::make_shared<frame_process_controller>(new anpr_worker(worker_config));
        connect(widget_stream_player_, &widget_stream_player::image_selected,
                process_controller_.get(), &frame_process_controller::predict);
        connect(process_controller_.get(), &frame_process_controller::send_process_results,
                this, &widget_stacks_anpr::display_frame);
        emit process_controller_->start();

        ui->pushButtonNext->setVisible(false);
    }
}

void widget_stacks_anpr::display_frame(std::any val)
{
    widget_stream_player_->display_frame(std::move(val));
}

void widget_stacks_anpr::init_stacked_widget()
{
    widget_object_detect_model_select_ = new widget_object_detect_model_select;
    widget_stream_player_ = new widget_stream_player;

    ui->stackedWidget->addWidget(widget_object_detect_model_select_);    
    ui->stackedWidget->addWidget(widget_stream_player_);

    ui->stackedWidget->setCurrentWidget(widget_object_detect_model_select_);
}

