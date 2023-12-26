#include "widget_stacks_faces_search.hpp"
#include "ui_widget_stacks_faces_search.h"

#include "widget_face_det_model_select.hpp"
#include "widget_face_recognition_model_select.hpp"
#include "widget_faces_register.hpp"
#include "widget_source_selection.hpp"

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

    ui->stackedWidget->addWidget(widget_face_det_model_select_);
    ui->stackedWidget->addWidget(widget_face_recognition_model_select_);
    ui->stackedWidget->addWidget(widget_faces_register_);
    ui->stackedWidget->addWidget(widget_source_selection_);

    ui->stackedWidget->setCurrentWidget(widget_faces_register_);
}

void widget_stacks_faces_search::next_page_is_widget_stream_player()
{

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
