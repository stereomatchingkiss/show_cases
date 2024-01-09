#include "widget_stacks_anpr.hpp"
#include "ui_widget_stacks_anpr.h"

#include "../algo/anpr_worker.hpp"

#include "../config/config_anpr_worker.hpp"
#include "../config/config_source_selection.hpp"

#include "widget_object_detect_model_select.hpp"
#include "widget_source_selection.hpp"
#include "widget_stream_player.hpp"

#include <multimedia/camera/frame_process_controller.hpp>
#include <multimedia/camera/single_frame_with_multi_worker_base.hpp>
#include <multimedia/network/frame_capture_websocket.hpp>
#include <multimedia/network/frame_capture_websocket_params.hpp>

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

    if(ui->stackedWidget->currentWidget() == widget_source_selection_){
        ui->stackedWidget->setCurrentWidget(widget_object_detect_model_select_);

        ui->pushButtonPrev->setVisible(false);
    }else if(ui->stackedWidget->currentWidget() == widget_stream_player_){
        ui->stackedWidget->setCurrentWidget(widget_source_selection_);

        sfwmw_ = nullptr;
        process_controller_ = nullptr;
    }
}

void widget_stacks_anpr::on_pushButtonNext_clicked()
{
    ui->pushButtonPrev->setVisible(true);

    if(ui->stackedWidget->currentWidget() == widget_object_detect_model_select_){
        ui->stackedWidget->setCurrentWidget(widget_source_selection_);
    }else if(ui->stackedWidget->currentWidget() == widget_source_selection_){
        ui->stackedWidget->setCurrentWidget(widget_stream_player_);

        widget_stream_player_->set_source_type(widget_source_selection_->get_config().source_type_);
        config_anpr_worker worker_config;
        worker_config.config_object_detect_model_select_ = widget_object_detect_model_select_->get_config();

        if(widget_source_selection_->get_config().source_type_ == stype::image){
            sfwmw_ = nullptr;
            process_controller_ = std::make_shared<frame_process_controller>(new anpr_worker(worker_config));
            connect(widget_stream_player_, &widget_stream_player::image_selected,
                    process_controller_.get(), &frame_process_controller::predict);
            connect(process_controller_.get(), &frame_process_controller::send_process_results,
                    this, &widget_stacks_anpr::display_frame);
            emit process_controller_->start();
        }else{
            //todo : finish websocket support
            process_controller_ = nullptr;
            sfwmw_ = std::make_unique<frame_capture_websocket>(widget_source_selection_->get_frame_capture_websocket_params());
            auto process_controller = std::make_shared<frame_process_controller>(new anpr_worker(worker_config));
            connect(process_controller.get(), &frame_process_controller::send_process_results,
                    widget_stream_player_, &widget_stream_player::display_frame);

            emit process_controller->start();
            sfwmw_->add_listener(process_controller, this);
            sfwmw_->start();
        }

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
    widget_source_selection_ = new widget_source_selection;
    widget_stream_player_ = new widget_stream_player;

    ui->stackedWidget->addWidget(widget_object_detect_model_select_);
    ui->stackedWidget->addWidget(widget_source_selection_);
    ui->stackedWidget->addWidget(widget_stream_player_);

    ui->stackedWidget->setCurrentWidget(widget_object_detect_model_select_);
}

