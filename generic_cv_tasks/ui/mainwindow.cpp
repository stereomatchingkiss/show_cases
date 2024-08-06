#include "mainwindow.hpp"
#include "./ui_mainwindow.h"

#include "dialog_alert_sender_settings.hpp"
#include "dialog_stream_select.hpp"

#include "widget_multi_stream_manager.hpp"
#include "widget_stacks_manager.hpp"

#include "../config/config_alert_sender.hpp"

#include "../global/global_object.hpp"

#include <network/websocket_client_controller.hpp>
#include <utils/unique_index.hpp>

#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , msg_box_{new QMessageBox(this)}
    , dialog_stream_select_{new dialog_stream_select(this)}
    , widget_multi_stream_manager_{nullptr}
{
    ui->setupUi(this);

    init_stacked_widget();
    get_alert_sound_manager();

    connect(ui->actionAddStream, &QAction::triggered, this, &MainWindow::action_add_stream);
    connect(ui->actionContactMe, &QAction::triggered, this, &MainWindow::action_contact_me);
    connect(ui->actionDeleteStream, &QAction::triggered, this, &MainWindow::action_delete_stream);
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

    setMinimumSize(QSize(1200, 800));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::action_about_qt(bool)
{
    msg_box_->aboutQt(this, tr("About Qt"));
}

void MainWindow::action_add_stream(bool)
{    
    auto *widget = new widget_stacks_manager;    
    widget->set_info_text(QString("cam_%1").arg(get_unique_index().get_and_update_unique_index()));
    widget_multi_stream_manager_->add_stream(widget);

    set_next_prev_button_visibility();
}

void MainWindow::action_contact_me(bool)
{
    msg_box_->setText(tr("Please send your email to thamngapwei@gmail.com.\n"
                         "Or open issue on\n"
                         "https://github.com/stereomatchingkiss/show_cases/issues"));

    msg_box_->show();
}

void MainWindow::action_delete_stream(bool)
{
    if(widget_multi_stream_manager_->get_stream_count() > 1){
        dialog_stream_select_->set_streams(widget_multi_stream_manager_->get_stream_names());
#ifdef WASM_BUILD
        dialog_stream_select_->show();
#else
        if(dialog_stream_select_->exec() == QDialog::Accepted){
            qDebug()<<__func__<<": dialog accepted";
            widget_multi_stream_manager_->delete_stream(dialog_stream_select_->get_selected_streams());
        }else{
            qDebug()<<__func__<<": dialog rejected";
        }
#endif
    }else{
        widget_multi_stream_manager_->delete_stream();
    }
    set_next_prev_button_visibility();
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
    widget_multi_stream_manager_ = new widget_multi_stream_manager;
    ui->stackedWidget->addWidget(widget_multi_stream_manager_);

    set_next_prev_button_visibility();
}

void MainWindow::load_settings(bool)
{
    widget_multi_stream_manager_->load_settings();

    set_next_prev_button_visibility();
}

void MainWindow::save_settings(bool)
{
    widget_multi_stream_manager_->save_settings();
}

void MainWindow::set_next_prev_button_visibility()
{
    ui->pushButtonNext->setVisible(widget_multi_stream_manager_->get_stream_page() < widget_multi_stream_manager_->get_max_page());
    ui->pushButtonPrev->setVisible(widget_multi_stream_manager_->get_stream_page() > 0);
}

void MainWindow::on_pushButtonPrev_clicked()
{
    widget_multi_stream_manager_->prev_page();
    set_next_prev_button_visibility();
}

void MainWindow::on_pushButtonNext_clicked()
{
    widget_multi_stream_manager_->next_page();
    set_next_prev_button_visibility();
}

