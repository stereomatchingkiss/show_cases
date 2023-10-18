#include "widget_source_selection.hpp"
#include "ui_widget_source_selection.h"

#include <utils/source_type.hpp>

#include <QFile>
#include <QFileDialog>

widget_source_selection::widget_source_selection(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::widget_source_selection)
{
    ui->setupUi(this);
}

widget_source_selection::~widget_source_selection()
{
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

void widget_source_selection::on_pushButtonOpenVideoFolder_clicked()
{
    if(auto const fname = QFileDialog::getOpenFileName(this, tr("Select Video"));
            !fname.isEmpty() && QFile(fname).exists())
    {
        ui->lineEditVideo->setText(fname);
        ui->radioButtonVideo->setChecked(true);
    }
}

