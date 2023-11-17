#include "mainwindow.hpp"
#include "./ui_mainwindow.h"

#include "widget_source_selection.hpp"
#include "widget_stream_player.hpp"

#include "../config/config_read_write.hpp"
#include "../global/global_keywords.hpp"

#include <QJsonObject>

#include <QFileDialog>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , msg_box_{new QMessageBox{this}}
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

    ui->pushButtonPrev->setEnabled(false);
}

MainWindow::~MainWindow()
{
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
                         "https://github.com/stereomatchingkiss/ocr_tasks/issues"));

    msg_box_->show();
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
    msg_box_->warning(this, tr("Before you use"),
                      tr("1. The software is copyrighted by the software developer.\n"
                         "2. Except for direct sale, the software can be used for personal or commercial purposes.\n"
                         "3. When using the software, please comply with relevant laws and regulations. "
                         "The software developer is not responsible for any loss or damage caused by the "
                         "use of this software."));
}

void MainWindow::init_widgets_states(QJsonObject const &jobj)
{
    global_keywords gk;
    widget_source_selection_->set_states(jobj[gk.state_widget_source_selection()].toObject());
}

QJsonObject MainWindow::dump_settings() const
{
    config_read_write crw;
    crw.set_widget_source_selection(widget_source_selection_->get_states());

    return crw.dumps();
}

void MainWindow::save_settings_to_file(QString const &save_at) const
{
    config_read_write().write(dump_settings(), save_at);
}


void MainWindow::on_pushButtonNext_clicked()
{
    if(ui->stackedWidget->currentWidget() == widget_source_selection_){
        ui->stackedWidget->setCurrentWidget(widget_stream_player_);
        ui->pushButtonNext->setEnabled(false);
        ui->pushButtonPrev->setEnabled(true);
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

