#include "mainwindow.hpp"
#include "./ui_mainwindow.h"

#include "widget_source_selection.hpp"
#include "widget_stream_player.hpp"

#include "../algo/frame_display_worker.hpp"

#include <multimedia/camera/frame_process_controller.hpp>
#include <multimedia/camera/frame_capture_params.hpp>
#include <multimedia/camera/single_frame_with_multi_worker.hpp>

using namespace ocv;

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
        frame_capture_params params;
        params.max_fps_ = 30;
        params.url_ = "D:/programming/videos/mode_0_1.mp4"; //url.toStdString();
        sfwmw_ = std::make_unique<single_frame_with_multi_worker>(params);
        auto process_controller = std::make_shared<frame_process_controller>(new frame_display_worker);
        connect(process_controller.get(), &frame_process_controller::send_process_results,
                widget_stream_player_, &widget_stream_player::display_frame);
        sfwmw_->add_listener(process_controller, this);
        sfwmw_->start();
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
