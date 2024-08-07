#include "widget_source_selection.hpp"
#include "ui_widget_source_selection.h"

#include "../config/config_source_selection.hpp"

#include <multimedia/camera/frame_capture_opencv_params.hpp>
#include <multimedia/camera/frame_capture_qcamera_params.hpp>
#include <multimedia/camera/frame_capture_qmediaplayer_params.hpp>
#include <multimedia/network/frame_capture_websocket_params.hpp>
#include <multimedia/stream_enum.hpp>

#include <QFile>
#include <QFileDialog>
#include <QJsonObject>

namespace{

inline QString state_max_fps(){ return "state_max_fps"; }
inline QString state_hls_url(){ return "state_hls_url"; }
inline QString state_rtsp_url(){ return "state_rtsp_url"; }
inline QString state_source_type(){ return "state_source_type"; }
inline QString state_video_url(){ return "state_video_url"; }
inline QString state_webcam_index(){ return "state_webcam_index"; }
inline QString state_websocket_url(){ return "state_websocket_url"; }

inline QString state_version(){ return "state_version"; }

}

using namespace flt::mm;

widget_source_selection::widget_source_selection(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::widget_source_selection)
{
    ui->setupUi(this);

#ifdef WASM_BUILD
    ui->radioButtonHLS->setVisible(false);
    ui->lineEditHLS->setVisible(false);
    ui->radioButtonWebcam->setVisible(false);
    ui->comboBoxWebCam->setVisible(false);
    ui->radioButtonVideo->setVisible(false);
    ui->lineEditVideo->setVisible(false);
    ui->pushButtonOpenVideoFolder->setVisible(false);
    ui->radioButtonWebsockets->setChecked(true);

    ui->radioButtonRTSP->setVisible(false);
    ui->lineEditRTSP->setVisible(false);

    set_max_fps_visible();
#endif        

    update_webcam_box();

    connect(&devices_, &QMediaDevices::videoInputsChanged, this, &widget_source_selection::update_webcam_box);
    connect(ui->radioButtonHLS, &QRadioButton::clicked, this, &widget_source_selection::set_max_fps_visible);
    connect(ui->radioButtonRTSP, &QRadioButton::clicked, this, &widget_source_selection::set_max_fps_visible);
    connect(ui->radioButtonVideo, &QRadioButton::clicked, this, &widget_source_selection::set_max_fps_visible);
    connect(ui->radioButtonWebcam, &QRadioButton::clicked, this, &widget_source_selection::set_max_fps_visible);
    connect(ui->radioButtonWebsockets, &QRadioButton::clicked, this, &widget_source_selection::set_max_fps_visible);

    ui->radioButtonVideo->setToolTip(tr("Due to browser limitations, please do not open images that are too large."));

    set_max_fps_visible();
}

widget_source_selection::~widget_source_selection()
{    
    delete ui;
}

frame_capture_qmediaplayer_params widget_source_selection::get_frame_capture_qmediaplayer_params() const
{
    frame_capture_qmediaplayer_params params;
    params.max_fps_ = get_max_fps();
    params.video_contents_ = video_contents_;
    qDebug()<<__func__<<": params.video_contents len = "<<params.video_contents_.size();
    params.url_ = get_url();

    return params;
}

frame_capture_qcamera_params widget_source_selection::get_frame_capture_qcamera_params() const
{
    frame_capture_qcamera_params params;
    params.max_fps_ = ui->spinBoxMaxFPS->value();
    if(!cam_devices_.empty()){
        params.device_ = cam_devices_[ui->comboBoxWebCam->currentIndex()];
    }

    return params;
}

frame_capture_opencv_params widget_source_selection::get_frame_capture_rtsp_params() const
{
    frame_capture_opencv_params params;
    params.max_fps_ = ui->spinBoxMaxFPS->value();
    params.url_ = ui->lineEditRTSP->text().toStdString();

    return params;
}

frame_capture_websocket_params widget_source_selection::get_frame_capture_websocket_params() const noexcept
{
    frame_capture_websocket_params params;
    params.max_fps_ = ui->spinBoxMaxFPS->value();
    params.url_ = get_url();

    return params;
}

stream_source_type widget_source_selection::get_source_type() const noexcept
{
    using stype = stream_source_type;
    if(ui->radioButtonRTSP->isChecked()){
        return stype::rtsp;
    }else if(ui->radioButtonVideo->isChecked()){
        return stype::video;
    }else if(ui->radioButtonWebsockets->isChecked()){
        return stype::websocket;
    }else if(ui->radioButtonHLS->isChecked()){
        return stype::hls;
    }

    return stype::webcam;
}

bool widget_source_selection::get_is_valid_source() const noexcept
{
    //if(ui->radioButtonRTSP->isChecked() && ui->lineEditRTSP->text().startsWith("rtsp")){
    if(ui->radioButtonRTSP->isChecked()){
        return true;
    }
#ifndef WASM_BUILD
    if(ui->radioButtonVideo->isChecked() && QFile::exists(ui->lineEditVideo->text())){
        return true;
    }
#else
    if(ui->radioButtonVideo->isChecked()){
        return true;
    }
#endif
    if(ui->radioButtonWebcam->isChecked() && ui->comboBoxWebCam->count() > 0){
        return true;
    }

    {
        auto const hls_addr = ui->lineEditHLS->text();
        if(ui->radioButtonHLS->isChecked() && (hls_addr.startsWith("http") || hls_addr.startsWith("https"))){
            return true;
        }
    }

    if(ui->radioButtonWebsockets->isChecked() &&
        (ui->lineEditWebsockets->text().startsWith("ws") || ui->lineEditWebsockets->text().startsWith("wss"))){
        return true;
    }

    return false;
}

int widget_source_selection::get_max_fps() const noexcept
{
    return ui->spinBoxMaxFPS->value();
}

QString widget_source_selection::get_rtsp_source() const noexcept
{
    return ui->lineEditRTSP->text();
}

QString widget_source_selection::get_video_source() const noexcept
{
    return ui->lineEditVideo->text();
}

QString widget_source_selection::get_webcam() const noexcept
{
    return ui->comboBoxWebCam->currentText();
}

QString widget_source_selection::get_url() const noexcept
{
    if(ui->radioButtonHLS->isChecked()){
        return ui->lineEditHLS->text();
    }
    if(ui->radioButtonRTSP->isChecked()){
        return ui->lineEditRTSP->text();
    }

    if(ui->radioButtonVideo->isChecked()){
        return ui->lineEditVideo->text();
    }

    if(ui->radioButtonWebsockets->isChecked()){
        return ui->lineEditWebsockets->text();
    }
    if(ui->comboBoxWebCam->currentIndex() != -1){
        return QString::number(ui->comboBoxWebCam->currentIndex());
    }

    return "";
}

config_source_selection widget_source_selection::get_config() const
{
    config_source_selection config;
    config.source_type_ = get_source_type();
    config.max_fps_ = ui->spinBoxMaxFPS->value();
    config.rtsp_url_ = ui->lineEditRTSP->text();
    config.url_ = get_url();
    config.video_url_ = ui->lineEditVideo->text();
    config.webcam_url_ = QString::number(ui->comboBoxWebCam->currentIndex());

    return config;
}

QJsonObject widget_source_selection::get_states() const
{
    QJsonObject obj;
    obj[state_hls_url()] = ui->lineEditHLS->text();
    obj[state_max_fps()] = ui->spinBoxMaxFPS->value();
    obj[state_rtsp_url()] = ui->lineEditRTSP->text();
    obj[state_source_type()] = static_cast<int>(get_source_type());
    obj[state_video_url()] = ui->lineEditVideo->text();
    obj[state_webcam_index()] = ui->comboBoxWebCam->currentIndex();
    obj[state_websocket_url()] = ui->lineEditWebsockets->text();

    obj[state_version()] = "1.0";

    return obj;
}

void widget_source_selection::set_states(const QJsonObject &val)
{
    if(val.contains(state_hls_url())){
        ui->lineEditHLS->setText(val[state_hls_url()].toString());
    }
    if(val.contains(state_max_fps())){
        ui->spinBoxMaxFPS->setValue(val[state_max_fps()].toInt());
    }
    if(val.contains(state_rtsp_url())){
        ui->lineEditRTSP->setText(val[state_rtsp_url()].toString());
    }
    if(val.contains(state_source_type())){
        using stype = flt::mm::stream_source_type;
        switch(static_cast<stype>(val[state_source_type()].toInt())){
        case stype::hls:{
            ui->radioButtonHLS->setChecked(true);
            break;
        }
        case stype::rtsp:{
            ui->radioButtonRTSP->setChecked(true);
            break;
        }
        case stype::video:{
            ui->radioButtonVideo->setChecked(true);
            break;
        }
        case stype::webcam:{
            ui->radioButtonWebcam->setChecked(true);
            break;
        }
        case stype::websocket:{
            ui->radioButtonWebsockets->setChecked(true);
            break;
        }
        default:
            break;
        }
    }
    if(val.contains(state_video_url())){
        ui->lineEditVideo->setText(val[state_video_url()].toString());
    }
    if(val.contains(state_webcam_index())){
        if(auto const idx = val[state_webcam_index()].toInt(); idx > 0 && idx < ui->comboBoxWebCam->maxCount()){
            ui->comboBoxWebCam->setCurrentIndex(idx);
        }
    }if(val.contains(state_websocket_url())){
        ui->lineEditWebsockets->setText(val[state_websocket_url()].toString());
    }

    set_max_fps_visible();
}

void widget_source_selection::set_websocket_url(const QString &val)
{
    ui->lineEditWebsockets->setText(val);
}

void widget_source_selection::on_pushButtonOpenVideoFolder_clicked()
{    
#ifndef WASM_BUILD
    auto const abs_path = QFileInfo(ui->lineEditVideo->text()).absolutePath();
    if(auto const fname = QFileDialog::getOpenFileName(this, tr("Select Video"), abs_path);
        !fname.isEmpty() && QFile(fname).exists())
    {
        ui->lineEditVideo->setText(fname);
        ui->radioButtonVideo->setChecked(true);
    }
#else
    auto fcontent_ready = [this](const QString &fname, const QByteArray &fcontent) {
        ui->lineEditVideo->setText(QFileInfo(fname).fileName());
        qDebug()<<"video contents from source length = "<<fcontent.size();
        video_contents_ = fcontent;
    };
    QFileDialog::getOpenFileContent("Videos (*.mp4 *.avi *.wav)",  fcontent_ready);
#endif
}

void widget_source_selection::set_max_fps_visible(bool)
{
    ui->labelMaxFps->setVisible(!ui->radioButtonWebsockets->isChecked());
    ui->spinBoxMaxFPS->setVisible(!ui->radioButtonWebsockets->isChecked());
    ui->horizontalSliderMaxFPS->setVisible(!ui->radioButtonWebsockets->isChecked());
}

void widget_source_selection::update_webcam_box()
{
    ui->comboBoxWebCam->clear();
    cam_devices_ = QMediaDevices::videoInputs();
    for(auto const &val : cam_devices_){
        ui->comboBoxWebCam->addItem(val.description());
    }
}

