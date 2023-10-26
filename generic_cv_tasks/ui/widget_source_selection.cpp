#include "widget_source_selection.hpp"
#include "ui_widget_source_selection.h"

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

QString widget_source_selection::get_rtsp_source() const
{
    return ui->lineEditRTSP->text();
}

QString widget_source_selection::get_video_source() const
{
    return ui->lineEditVideo->text();
}

QString widget_source_selection::get_webcam() const
{
    return ui->comboBoxWebCam->currentText();
}

QString widget_source_selection::get_url() const
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
    if(auto const fname = QFileDialog::getOpenFileName(this, tr("Select Video"));
            !fname.isEmpty() && QFile(fname).exists())
    {
        ui->lineEditVideo->setText(fname);
        ui->radioButtonVideo->setChecked(true);
    }
}

