#include "mainwindow.hpp"
#include "./ui_mainwindow.h"

#include "widget_source_selection.hpp"
#include "widget_stream_player.hpp"

#include "../algo/paddle_ocr_worker.hpp"

#include "../config/config_paddle_ocr_worker.hpp"
#include "../config/config_read_write.hpp"
#include "../config/config_source_selection.hpp"

#include "../global/global_keywords.hpp"
#include "../global/global_object.hpp"

#include <multimedia/camera/frame_process_controller.hpp>
#include <multimedia/camera/single_frame_with_multi_worker_base.hpp>
#include <multimedia/network/frame_capture_websocket.hpp>
#include <multimedia/network/frame_capture_websocket_params.hpp>

#include <QJsonDocument>
#include <QJsonObject>

#include <QFileDialog>
#include <QMessageBox>

using namespace flt::mm;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    widget_source_selection_ = new widget_source_selection;
    widget_stream_player_ = new widget_stream_player;

    ui->stackedWidget->addWidget(widget_source_selection_);
    ui->stackedWidget->addWidget(widget_stream_player_);

    connect(ui->actionQt, &QAction::triggered, this, &MainWindow::action_about_qt);
    connect(ui->actionContactMe, &QAction::triggered, this, &MainWindow::action_contact_me);
    connect(ui->actionLoadSettings, &QAction::triggered, this, &MainWindow::action_load_settings);
    connect(ui->actionSaveSettings, &QAction::triggered, this, &MainWindow::action_save_settings);
    connect(ui->actionReadMe, &QAction::triggered, this, &MainWindow::action_warning);

    connect(widget_stream_player_, &widget_stream_player::process_done, [this]()
            {
                setEnabled(true);
#ifdef WASM_BUILD
                resize_window();
#endif
            });
#ifdef WASM_BUILD
    connect(widget_stream_player_, &widget_stream_player::resize_window, [this]()
            {
                resize_window();
            });
#endif

    ui->pushButtonPrev->setEnabled(false);    

#ifndef WASM_BUILD
    auto const jobj = config_read_write().read(global_keywords().cam_config_path() + "/cam0.json");
    global_keywords gk;
    widget_source_selection_->set_states(jobj[gk.state_widget_source_selection()].toObject());
    widget_stream_player_->set_states(jobj[gk.state_widget_stream_player()].toObject());
#endif

    get_gobject();
}

MainWindow::~MainWindow()
{
#ifndef WASM_BUILD
    config_read_write().write(dump_settings(), global_keywords().cam_config_path() + "/cam0.json");
#endif

    delete ui;
}

void MainWindow::action_about_qt(bool)
{
    get_gobject().messagebox().aboutQt(this, tr("About Qt"));
}

void MainWindow::action_contact_me(bool)
{
    get_gobject().messagebox().setText(tr("Please send your email to thamngapwei@gmail.com.\n"
                                          "Or open issue on\n"
                                          "https://github.com/stereomatchingkiss/ocr_tasks/issues"));

    get_gobject().messagebox().show();
}

void MainWindow::action_load_settings(bool)
{
#ifndef WASM_BUILD
    if(auto const fname = QFileDialog::getOpenFileName(this, tr("Select settings"), "", tr("Settings (*.json)"));
        !fname.isEmpty() && QFile(fname).exists())
    {
        init_widgets_states(config_read_write().read(fname));
    }
#else
    auto fcontent_ready = [this](QString const&, QByteArray const &fcontent) {
        init_widgets_states(config_read_write().read(fcontent));
    };
    QFileDialog::getOpenFileContent("Settings (*.json)",  fcontent_ready);
#endif
}

void MainWindow::action_save_settings(bool)
{
#ifndef WASM_BUILD
    if(auto const fname = QFileDialog::getSaveFileName(this, tr("Save at"));
        !fname.isEmpty() && QFile(fname).exists())
    {
        if(fname.right(4) == ".json"){
            save_settings_to_file(fname);
        }else{
            save_settings_to_file(fname + ".json");
        }
    }
#else
    QFileDialog::saveFileContent(QJsonDocument(dump_settings()).toJson(), "cam0.json");
#endif
}

void MainWindow::action_warning(bool)
{
    get_gobject().messagebox().warning(this, tr("Before you use"),
                                       tr("1. The software is copyrighted by the software developer.\n"
                                          "2. Except for direct sale, the software can be used for personal or commercial purposes.\n"
                                          "3. When using the software, please comply with relevant laws and regulations. "
                                          "The software developer is not responsible for any loss or damage caused by the "
                                          "use of this software."));
}

void MainWindow::display_frame(std::any val)
{
    setEnabled(false);
    widget_stream_player_->display_frame(std::move(val));
}

void MainWindow::init_widgets_states(QJsonObject const &jobj)
{
    global_keywords gk;
    widget_source_selection_->set_states(jobj[gk.state_widget_source_selection()].toObject());
    widget_stream_player_->set_states(jobj[gk.state_widget_stream_player()].toObject());
}

void MainWindow::resize_window()
{
    showMaximized();
}

QJsonObject MainWindow::dump_settings() const
{
    config_read_write crw;
    crw.set_widget_source_selection(widget_source_selection_->get_states());
    crw.set_widget_stream_player(widget_stream_player_->get_states());

    return crw.dumps();
}

void MainWindow::save_settings_to_file(QString const &save_at) const
{
    config_read_write().write(dump_settings(), save_at);
}

void MainWindow::on_pushButtonNext_clicked()
{
    if(ui->stackedWidget->currentWidget() == widget_source_selection_){
        ui->labelTitle->setVisible(false);
        ui->stackedWidget->setCurrentWidget(widget_stream_player_);
        ui->pushButtonNext->setEnabled(false);
        ui->pushButtonPrev->setEnabled(true);

        config_paddle_ocr_worker worker_config;
        worker_config.config_source_selection_ = widget_source_selection_->get_config();
        if(widget_source_selection_->get_config().source_type_ == stream_source_type::image){
            widget_stream_player_->set_can_save_on_local(true);

            sfwmw_ = nullptr;
            process_controller_ = std::make_shared<frame_process_controller>(new paddle_ocr_worker(worker_config));
            connect(widget_stream_player_, &widget_stream_player::image_selected,
                    process_controller_.get(), &frame_process_controller::predict);
            connect(process_controller_.get(), &frame_process_controller::send_process_results,
                    this, &MainWindow::display_frame);
            emit process_controller_->start();
        }else{
            widget_stream_player_->set_can_save_on_local(false);

            process_controller_ = nullptr;
            sfwmw_ = std::make_unique<frame_capture_websocket>(widget_source_selection_->get_frame_capture_websocket_params());
            auto process_controller = std::make_shared<frame_process_controller>(new paddle_ocr_worker(worker_config));
            connect(process_controller.get(), &frame_process_controller::send_process_results,
                    widget_stream_player_, &widget_stream_player::display_frame);
            connect(widget_stream_player_,
                    &widget_stream_player::send_ocr_results,
                    static_cast<frame_capture_websocket*>(sfwmw_.get()),
                    &frame_capture_websocket::send_text_message);

            emit process_controller->start();
            sfwmw_->add_listener(process_controller, this);
            sfwmw_->start();
        }
    }

    resize_window();
}


void MainWindow::on_pushButtonPrev_clicked()
{
    if(ui->stackedWidget->currentWidget() == widget_stream_player_){
        sfwmw_ = nullptr;
        process_controller_ = nullptr;

        ui->labelTitle->setVisible(true);
        ui->stackedWidget->setCurrentWidget(widget_source_selection_);
        ui->pushButtonNext->setEnabled(true);
        ui->pushButtonPrev->setEnabled(false);
    }

    resize_window();
}

