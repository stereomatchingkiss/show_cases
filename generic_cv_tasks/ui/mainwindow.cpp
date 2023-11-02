#include "mainwindow.hpp"
#include "./ui_mainwindow.h"

#include "widget_object_detect_model_select.hpp"
#include "widget_select_object_to_detect.hpp"
#include "widget_source_selection.hpp"
#include "widget_stream_player.hpp"
#include "widget_tracker_alert.hpp"

#include "../algo/frame_display_worker.hpp"
#include "../algo/obj_detect/nanodet_worker.hpp"

#include "../config/config_nanodet_worker.hpp"
#include "../config/config_read_write.hpp"

#include "../global/global_keywords.hpp"

#include <multimedia/camera/frame_process_controller.hpp>
#include <multimedia/camera/frame_capture_params.hpp>
#include <multimedia/camera/single_frame_with_multi_worker.hpp>

#include <ui/label_select_roi.hpp>

#include <QMessageBox>

using namespace flt;
using namespace flt::mm;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    init_stacked_widget();

    ui->pushButtonPrev->setEnabled(false);
    ui->labelTitle->setText(tr("Select model"));

    connect(ui->actionQt, &QAction::triggered, [](bool)
            {
        QMessageBox::aboutQt(nullptr, tr("About Qt"));
    });

    setMinimumSize(QSize(600, 400));
}

MainWindow::~MainWindow()
{    
    config_read_write crw;
    crw.set_roi(label_select_roi_->get_states());
    crw.set_widget_object_detect_model_select(widget_object_detect_model_select_->get_states());
    crw.set_widget_select_object_to_detect(widget_select_object_to_detect_->get_states());
    crw.set_widget_source_selection(widget_source_selection_->get_states());
    crw.write(global_keywords().cam_config_path() + "/cam0.json");

    delete ui;
}

void MainWindow::on_pushButtonNext_clicked()
{
    if(ui->stackedWidget->currentWidget() == widget_object_detect_model_select_){
        next_page_is_widget_select_object_to_detect();
    }else if(ui->stackedWidget->currentWidget() == widget_select_object_to_detect_){
        next_page_is_widget_tracker_alert();
    }else if(ui->stackedWidget->currentWidget() == widget_tracker_alert_){
        next_page_is_widget_source_selection();
    }else if(ui->stackedWidget->currentWidget() == widget_source_selection_){
        next_page_is_label_select_roi();
    }else if(ui->stackedWidget->currentWidget() == label_select_roi_){
        next_page_is_widget_stream_player();
    }
}


void MainWindow::on_pushButtonPrev_clicked()
{
    sfwmw_.reset();
    ui->labelTitle->setVisible(true);
    if(ui->stackedWidget->currentWidget() == widget_select_object_to_detect_){
        ui->labelTitle->setText(tr("Select model"));
        ui->stackedWidget->setCurrentWidget(widget_object_detect_model_select_);
        ui->pushButtonNext->setEnabled(true);
        ui->pushButtonPrev->setEnabled(false);
    }else if(ui->stackedWidget->currentWidget() == widget_source_selection_){
        ui->labelTitle->setText(tr("Alert settings"));
        ui->stackedWidget->setCurrentWidget(widget_tracker_alert_);
        ui->pushButtonNext->setEnabled(true);
        ui->pushButtonPrev->setEnabled(true);
    }else if(ui->stackedWidget->currentWidget() == widget_tracker_alert_){
        ui->labelTitle->setText(tr("Select objects to detect"));
        ui->stackedWidget->setCurrentWidget(widget_select_object_to_detect_);
        ui->pushButtonNext->setEnabled(true);
        ui->pushButtonPrev->setEnabled(true);
    }else if(ui->stackedWidget->currentWidget() == label_select_roi_){
        ui->labelTitle->setText(tr("Select stream"));
        ui->stackedWidget->setCurrentWidget(widget_source_selection_);
        ui->pushButtonNext->setEnabled(true);
        ui->pushButtonPrev->setEnabled(true);
    }else if(ui->stackedWidget->currentWidget() == widget_stream_player_){
        ui->labelTitle->setText(tr("Select roi"));
        ui->stackedWidget->setCurrentWidget(label_select_roi_);
        ui->pushButtonNext->setEnabled(true);
        ui->pushButtonPrev->setEnabled(true);

        create_roi_select_stream();
    }
}

void MainWindow::create_roi_select_stream()
{
    sfwmw_ = std::make_unique<single_frame_with_multi_worker>(widget_source_selection_->get_frame_capture_params());
    auto process_controller = std::make_shared<frame_process_controller>(new frame_display_worker);
    connect(process_controller.get(), &frame_process_controller::send_process_results,
            label_select_roi_, &ui::label_select_roi::display_frame);
    sfwmw_->add_listener(process_controller, this);
    sfwmw_->start();
}

void MainWindow::init_stacked_widget()
{
    label_select_roi_ = new ui::label_select_roi(tr("Select roi"));
    widget_object_detect_model_select_ = new widget_object_detect_model_select;
    widget_source_selection_ = new widget_source_selection;
    widget_stream_player_    = new widget_stream_player;
    widget_tracker_alert_    = new widget_tracker_alert;

    global_keywords gk;
    widget_select_object_to_detect_ = new widget_select_object_to_detect(gk.coco_names());

    config_read_write crw;
    auto const jobj = crw.read(global_keywords().cam_config_path() + "/cam0.json");

    label_select_roi_->set_states(jobj[gk.state_roi()].toObject());
    widget_object_detect_model_select_->set_states(jobj[gk.state_widget_object_detect_model_select()].toObject());
    widget_select_object_to_detect_->set_states(jobj[gk.state_widget_select_object_to_detect()].toObject());
    widget_source_selection_->set_states(jobj[gk.state_widget_source_selection()].toObject());
    widget_tracker_alert_->set_states(jobj[gk.state_tracker_alert()].toObject());

    ui->stackedWidget->addWidget(label_select_roi_);
    ui->stackedWidget->addWidget(widget_object_detect_model_select_);
    ui->stackedWidget->addWidget(widget_select_object_to_detect_);
    ui->stackedWidget->addWidget(widget_source_selection_);
    ui->stackedWidget->addWidget(widget_stream_player_);
    ui->stackedWidget->addWidget(widget_tracker_alert_);

    ui->stackedWidget->setCurrentWidget(widget_object_detect_model_select_);
}

void MainWindow::next_page_is_widget_stream_player()
{
    ui->labelTitle->setVisible(false);
    ui->stackedWidget->setCurrentWidget(widget_stream_player_);
    ui->pushButtonNext->setEnabled(false);
    ui->pushButtonPrev->setEnabled(true);    

    config_nanodet_worker config;
    config.config_object_detect_model_select_ = widget_object_detect_model_select_->get_config();
    config.config_select_object_to_detect_ = widget_select_object_to_detect_->get_config();
    config.roi_ = label_select_roi_->get_norm_rubber_band_rect();
    auto process_controller =
        std::make_shared<frame_process_controller>(new nanodet_worker(std::move(config)));
    connect(process_controller.get(), &frame_process_controller::send_process_results,
            widget_stream_player_, &widget_stream_player::display_frame);

    sfwmw_ = std::make_unique<single_frame_with_multi_worker>(widget_source_selection_->get_frame_capture_params());
    sfwmw_->add_listener(process_controller, this);
    sfwmw_->start();
}

void MainWindow::next_page_is_widget_select_object_to_detect()
{
    ui->labelTitle->setText(tr("Select objects to detect"));
    ui->stackedWidget->setCurrentWidget(widget_select_object_to_detect_);
    ui->pushButtonNext->setEnabled(true);
    ui->pushButtonPrev->setEnabled(true);
}

void MainWindow::next_page_is_widget_source_selection()
{
    ui->labelTitle->setText(tr("Select steram"));
    ui->stackedWidget->setCurrentWidget(widget_source_selection_);
    ui->pushButtonNext->setEnabled(true);
    ui->pushButtonPrev->setEnabled(true);
}

void MainWindow::next_page_is_widget_tracker_alert()
{
    ui->labelTitle->setText(tr("Alert settings"));
    ui->stackedWidget->setCurrentWidget(widget_tracker_alert_);
    ui->pushButtonNext->setEnabled(true);
    ui->pushButtonPrev->setEnabled(true);
}

void MainWindow::next_page_is_label_select_roi()
{
    if(!widget_source_selection_->get_is_valid_source()){

    }
    ui->labelTitle->setText(tr("Select roi"));
    ui->stackedWidget->setCurrentWidget(label_select_roi_);
    ui->pushButtonNext->setEnabled(true);
    ui->pushButtonPrev->setEnabled(true);

    create_roi_select_stream();
}
