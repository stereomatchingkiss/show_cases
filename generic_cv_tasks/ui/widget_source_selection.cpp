#include "widget_source_selection.hpp"
#include "ui_widget_source_selection.h"

#include <multimedia/camera/frame_capture_params.hpp>
#include <multimedia/stream_enum.hpp>

#include <QFile>
#include <QFileDialog>
#include <QSettings>

namespace{

char const *rtsp_url("rtsp_url");
char const *video_url("video_url");

}

widget_source_selection::widget_source_selection(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::widget_source_selection)
{
    ui->setupUi(this);

    QSettings setting;
    if(setting.contains(rtsp_url)){
        ui->lineEditRTSP->setText(setting.value(rtsp_url).toString());
    }
    if(setting.contains(video_url)){
        ui->lineEditVideo->setText(setting.value(video_url).toString());
    }
}

widget_source_selection::~widget_source_selection()
{
    QSettings setting;
    setting.setValue(rtsp_url, ui->lineEditRTSP->text());
    setting.setValue(video_url, ui->lineEditVideo->text());

    delete ui;
}

ocv::frame_capture_params widget_source_selection::get_frame_capture_params() const
{
    ocv::frame_capture_params params;
    params.max_fps_ = get_max_fps();
    params.receiver_type_ = ocv::stream_receiver_type::opencv;
    params.src_type_ = get_source_type();
    params.url_ = get_url().toStdString();

    return params;
}

ocv::stream_source_type widget_source_selection::get_source_type() const noexcept
{
    using stype = ocv::stream_source_type;
    if(ui->radioButtonRTSP->isChecked()){
        return stype::rtsp;
    }else if(ui->radioButtonVideo->isChecked()){
        return stype::video;
    }

    return stype::webcam;
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
    if(ui->radioButtonRTSP->isChecked()){
        return ui->lineEditRTSP->text();
    }

    if(ui->radioButtonVideo->isChecked()){
        return ui->lineEditVideo->text();
    }

    return "0";
}

void widget_source_selection::on_pushButtonOpenVideoFolder_clicked()
{
    auto const abs_path = QFileInfo(ui->lineEditVideo->text()).absolutePath();
    if(auto const fname = QFileDialog::getOpenFileName(this, tr("Select Video"), abs_path);
            !fname.isEmpty() && QFile(fname).exists())
    {
        ui->lineEditVideo->setText(fname);
        ui->radioButtonVideo->setChecked(true);
    }
}

