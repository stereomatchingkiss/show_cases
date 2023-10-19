#include "mainwindow.hpp"
#include "./ui_mainwindow.h"

#include "widget_source_selection.hpp"
#include "widget_stream_player.hpp"

#include "../algo/frame_display_worker.hpp"

#include <multimedia/camera/frame_capture_params.hpp>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    widget_source_selection_ = new widget_source_selection(this);
    widget_stream_player_    = new widget_stream_player(this);

    ui->stackedWidget->addWidget(widget_source_selection_);
    ui->stackedWidget->addWidget(widget_stream_player_);

    ui->pushButtonPrev->setEnabled(false);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pushButtonNext_clicked()
{
    if(ui->stackedWidget->currentWidget() == widget_source_selection_){
        ui->stackedWidget->setCurrentWidget(widget_stream_player_);
        ui->pushButtonNext->setEnabled(false);
        ui->pushButtonPrev->setEnabled(true);
        ocv::frame_capture_params params;
        params.max_fps_ = 30;
        params.url_ = "D:/programming/videos/mode_0_0.mp4"; //url.toStdString();
        auto *worker = new frame_display_worker;
        widget_stream_player_->play(params, worker);
    }
}


void MainWindow::on_pushButtonPrev_clicked()
{
    if(ui->stackedWidget->currentWidget() == widget_stream_player_){
        ui->stackedWidget->setCurrentWidget(widget_source_selection_);
        ui->pushButtonNext->setEnabled(true);
        ui->pushButtonPrev->setEnabled(false);
    }
}

