#include "mainwindow.hpp"
#include "./ui_mainwindow.h"

#include "widget_alert_sender_settings.hpp"

#include "widget_stacks_manager.hpp"

#include "../config/config_alert_sender.hpp"
#include "../config/config_read_write.hpp"

#include "../global/global_keywords.hpp"
#include "../global/global_object.hpp"

#include <network/websocket_client_controller.hpp>

#include <QFileDialog>
#include <QMessageBox>

#include <QJsonDocument>
#include <QJsonObject>

using namespace flt;
using namespace flt::mm;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)    
    , msg_box_(new QMessageBox(this))
{
    ui->setupUi(this);

    init_stacked_widget();

    connect(ui->actionContactMe, &QAction::triggered, this, &MainWindow::action_contact_me);
    connect(ui->actionQt, &QAction::triggered, this, &MainWindow::action_about_qt);
    connect(ui->actionLoadSettings, &QAction::triggered, this, &MainWindow::load_settings);
    connect(ui->actionSaveSettings, &QAction::triggered, this, &MainWindow::save_settings);
    connect(ui->actionReadMe, &QAction::triggered, this, &MainWindow::action_warning);
    connect(ui->actionServer, &QAction::triggered, this, &MainWindow::action_server_call);
    connect(&get_widget_alert_sender_settings(), &widget_alert_sender_settings::button_ok_clicked, [](auto const &val)
            {
                if(get_widget_alert_sender_settings().get_config().activate_){
                    emit get_websocket_controller().reopen_if_needed(val.url_);
                }
            });

    emit get_websocket_controller().initialize();

    setMinimumSize(QSize(600, 400));    
}

MainWindow::~MainWindow()
{    
    save_settings_to_file(global_keywords().cam_config_path() + "/cam0.json");

    delete ui;
}

void MainWindow::action_about_qt(bool)
{
    msg_box_->aboutQt(this, tr("About Qt"));
}

void MainWindow::action_contact_me(bool)
{
    msg_box_->setText(tr("Please send your email to thamngapwei@gmail.com.\n"
                         "Or open issue on\n"
                         "https://github.com/stereomatchingkiss/object_detection_and_alarm/issues"));

    msg_box_->show();
}

void MainWindow::action_server_call()
{
    get_widget_alert_sender_settings().show();
}

void MainWindow::action_warning(bool)
{
    msg_box_->warning(this, tr("Before you use"),
                      tr("1. The software is copyrighted by the software developer.\n"
                         "2. Except for direct sale, the software can be used for personal or commercial purposes.\n"
                         "3. When using the software, please comply with relevant laws and regulations. "
                         "The software developer is not responsible for any loss or damage caused by the "
                         "use of this software."));
}



void MainWindow::init_stacked_widget()
{    
    widget_stacks_manager_ = new widget_stacks_manager;
    ui->stackedWidget->addWidget(widget_stacks_manager_);
}

QJsonObject MainWindow::dump_settings() const
{
    config_read_write crw;    
    crw.set_widget_alert_settings(get_widget_alert_sender_settings().get_states());

    return crw.dumps();
}

void MainWindow::load_settings(bool)
{
#ifndef WASM_BUILD
    if(auto const fname = QFileDialog::getOpenFileName(this, tr("Select settings"), "", tr("Settings (*.json)"));
        !fname.isEmpty() && QFile(fname).exists())
    {
        init_widgets_states(fname);
    }
#else
    auto fcontent_ready = [this](QString const&, QByteArray const &fcontent) {
        init_widgets_states(config_read_write().read(fcontent));
    };
    QFileDialog::getOpenFileContent("Settings (*.json)",  fcontent_ready);
#endif
}

void MainWindow::init_widgets_states(const QString &fname)
{    
    init_widgets_states(config_read_write().read(fname));
}

void MainWindow::init_widgets_states(const QJsonObject &jobj)
{
    global_keywords gk;    
    get_widget_alert_sender_settings().set_states(jobj[gk.state_widget_alert_settings()].toObject());
}

void MainWindow::save_settings(bool)
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

void MainWindow::save_settings_to_file(const QString &save_at) const
{
    config_read_write().write(dump_settings(), save_at);
}
