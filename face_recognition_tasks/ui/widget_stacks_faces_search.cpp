#include "widget_stacks_faces_search.hpp"
#include "ui_widget_stacks_faces_search.h"

#include "widget_face_det_model_select.hpp"
#include "widget_face_recognition_model_select.hpp"
#include "widget_faces_register.hpp"
#include "widget_source_selection.hpp"
#include "widget_stream_player.hpp"

#include "../algo/face_search/face_search_worker.hpp"

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

namespace{



}

using namespace flt::mm;

widget_stacks_faces_search::widget_stacks_faces_search(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::widget_stacks_faces_search)
{
    ui->setupUi(this);

    init_stacked_widget();
}

widget_stacks_faces_search::~widget_stacks_faces_search()
{
    delete ui;
}

void widget_stacks_faces_search::on_pushButtonPrev_clicked()
{

}


void widget_stacks_faces_search::on_pushButtonNext_clicked()
{
    if(ui->stackedWidget->currentWidget() == widget_faces_register_){
        ui->stackedWidget->setCurrentWidget(widget_source_selection_);
    }
}

void widget_stacks_faces_search::create_frame_capture()
{
    if(widget_source_selection_->get_source_type() == stream_source_type::websocket){
        sfwmw_ = std::make_unique<frame_capture_websocket>(widget_source_selection_->get_frame_capture_websocket_params());
    }
}

void widget_stacks_faces_search::init_stacked_widget()
{
    widget_face_det_model_select_ = new widget_face_det_model_select;
    widget_face_recognition_model_select_ = new widget_face_recognition_model_select;
    widget_faces_register_ = new widget_faces_register;
    widget_source_selection_ = new widget_source_selection;
    widget_stream_player_ = new widget_stream_player;

    connect(widget_stream_player_, &widget_stream_player::send_text_msg,
            this, &widget_stacks_faces_search::send_msg_by_text);

    ui->stackedWidget->addWidget(widget_face_det_model_select_);
    ui->stackedWidget->addWidget(widget_face_recognition_model_select_);
    ui->stackedWidget->addWidget(widget_faces_register_);
    ui->stackedWidget->addWidget(widget_source_selection_);
    ui->stackedWidget->addWidget(widget_stream_player_);

    ui->stackedWidget->setCurrentWidget(widget_faces_register_);

    ui->pushButtonPrev->setVisible(false);
}

void widget_stacks_faces_search::next_page_is_widget_stream_player()
{
    ui->stackedWidget->setCurrentWidget(widget_stream_player_);
    ui->pushButtonNext->setVisible(false);

    auto worker = new face_search_worker;
    auto process_controller = std::make_shared<frame_process_controller>(worker);
    connect(process_controller.get(), &frame_process_controller::send_process_results,
            widget_stream_player_, &widget_stream_player::display_frame);

    create_frame_capture();
    emit process_controller->start();
    sfwmw_->add_listener(process_controller, this);
    sfwmw_->start();
}

void widget_stacks_faces_search::send_msg_by_binary(const QByteArray &msg)
{
    if(sfwmw_){
        static_cast<frame_capture_websocket*>(sfwmw_.get())->send_text_message(msg);
    }
}

void widget_stacks_faces_search::send_msg_by_text(const QString &msg)
{
    if(sfwmw_){
        static_cast<frame_capture_websocket*>(sfwmw_.get())->send_text_message(msg);
    }
}

