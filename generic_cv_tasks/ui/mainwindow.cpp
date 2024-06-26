#include "mainwindow.hpp"
#include "./ui_mainwindow.h"

#include "dialog_alert_sender_settings.hpp"

#include "widget_stacks_manager.hpp"

#include "../config/config_alert_sender.hpp"

#include "../global/global_keywords.hpp"
#include "../global/global_object.hpp"

#include <json/json_utils.hpp>
#include <network/websocket_client_controller.hpp>

#include <QFileDialog>
#include <QMessageBox>

#include <QJsonArray>
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
    connect(&get_widget_alert_sender_settings(), &dialog_alert_sender_settings::button_ok_clicked, [](auto const &val)
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

    init_widgets_states(global_keywords().cam_config_path() + "/cam0.json");
}

QJsonArray MainWindow::dump_stacks_states() const
{
    QJsonObject cam_state_obj;
    global_keywords const gk;
    cam_state_obj[gk.cam_name()] = "cam0";
    cam_state_obj[gk.state_stacks_manager()] = widget_stacks_manager_->get_states();

    QJsonObject obj_out;
    QJsonArray arr;
    arr.append(cam_state_obj);

    return arr;
}

QJsonObject MainWindow::dump_settings() const
{    
    global_keywords const gk;
    QJsonObject obj_out;

    obj_out[gk.state_cam_states()] = dump_stacks_states();
    obj_out[gk.state_version()] = "2.0";
    obj_out[gk.state_widget_alert_settings()] = get_widget_alert_sender_settings().get_states();

    return obj_out;
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
        init_widgets_states(QJsonDocument::fromJson(fcontent).object());
    };
    QFileDialog::getOpenFileContent("Settings (*.json)",  fcontent_ready);
#endif
}

void MainWindow::init_widgets_states(const QString &fname)
{    
    init_widgets_states(flt::json::parse_file_to_jobj(fname));
}

void MainWindow::init_widgets_states(const QJsonObject &jobj)
{
    global_keywords const gk;
    if(jobj.contains(gk.state_cam_states())){
        if(auto const arr = jobj[gk.state_cam_states()].toArray(); !arr.empty()){
            for(size_t i = 0; i != arr.size(); ++i){
                auto const &aobj = arr[i].toObject();
                if(aobj.contains(gk.state_stacks_manager())){
                    widget_stacks_manager_->set_states(aobj[gk.state_stacks_manager()].toObject());
                }
            }
        }
    }

    if(jobj.contains(gk.state_widget_alert_settings())){
        get_widget_alert_sender_settings().set_states(jobj[gk.state_widget_alert_settings()].toObject());
    }

    widget_stacks_manager_->switch_to_task_selection_page();
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
    flt::json::write_file_to_json(dump_settings(), save_at);
}
