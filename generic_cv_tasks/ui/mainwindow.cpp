#include "mainwindow.hpp"
#include "./ui_mainwindow.h"

#include "widget_source_selection.hpp"
#include "widget_stream_player.hpp"

#include "../algo/frame_display_worker.hpp"
#include "../algo/obj_detect/nanodet_worker.hpp"

#include "../global/global_keywords.hpp"

#include <multimedia/camera/frame_process_controller.hpp>
#include <multimedia/camera/frame_capture_params.hpp>
#include <multimedia/camera/single_frame_with_multi_worker.hpp>

#include <ui/label_select_roi.hpp>

using namespace ocv;
using namespace ocv::mm;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    label_select_roi_ = new ui::label_select_roi(tr("Select roi"), global_keywords().label_select_roi_path());
    widget_source_selection_ = new widget_source_selection;
    widget_stream_player_    = new widget_stream_player;

    ui->stackedWidget->addWidget(widget_source_selection_);
    ui->stackedWidget->addWidget(label_select_roi_);
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
        ui->labelTitle->setText(tr("Select roi"));
        ui->stackedWidget->setCurrentWidget(label_select_roi_);
        ui->pushButtonNext->setEnabled(true);
        ui->pushButtonPrev->setEnabled(true);

        sfwmw_ = std::make_unique<single_frame_with_multi_worker>(widget_source_selection_->get_frame_capture_params());
        auto process_controller = std::make_shared<frame_process_controller>(new frame_display_worker);
        connect(process_controller.get(), &frame_process_controller::send_process_results,
                label_select_roi_, &ui::label_select_roi::display_frame);
        sfwmw_->add_listener(process_controller, this);
        sfwmw_->start();
    }else if(ui->stackedWidget->currentWidget() == label_select_roi_){
        ui->labelTitle->setText(tr("Display"));
        ui->stackedWidget->setCurrentWidget(widget_stream_player_);
        ui->pushButtonNext->setEnabled(false);
        ui->pushButtonPrev->setEnabled(true);

        sfwmw_ = std::make_unique<single_frame_with_multi_worker>(widget_source_selection_->get_frame_capture_params());
        auto process_controller =
            std::make_shared<frame_process_controller>(new nanodet_worker(label_select_roi_->get_rubber_band_rect(), 0.25));
        connect(process_controller.get(), &frame_process_controller::send_process_results,
                widget_stream_player_, &widget_stream_player::display_frame);
        sfwmw_->add_listener(process_controller, this);
        sfwmw_->start();
    }
}


void MainWindow::on_pushButtonPrev_clicked()
{
    if(ui->stackedWidget->currentWidget() == label_select_roi_){
        sfwmw_.reset();
        ui->labelTitle->setText(tr("Select stream"));
        ui->stackedWidget->setCurrentWidget(widget_source_selection_);
        ui->pushButtonNext->setEnabled(true);
        ui->pushButtonPrev->setEnabled(false);
    }else if(ui->stackedWidget->currentWidget() == widget_stream_player_){
        ui->labelTitle->setText(tr("Select roi"));
        ui->stackedWidget->setCurrentWidget(label_select_roi_);
        ui->pushButtonNext->setEnabled(true);
        ui->pushButtonPrev->setEnabled(true);
    }
}
