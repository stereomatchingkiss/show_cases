#include "mainwindow.hpp"
#include "./ui_mainwindow.h"

#include "widget_stacks_manager.hpp"

#include <QJsonArray>
#include <QJsonObject>

#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow),
    msg_box_(new QMessageBox(this))
{
    ui->setupUi(this);

    connect(ui->actionContactMe, &QAction::triggered, this, &MainWindow::action_contact_me);
    connect(ui->actionQt, &QAction::triggered, this, &MainWindow::action_about_qt);
    connect(ui->actionLoadSettings, &QAction::triggered, this, &MainWindow::load_settings);
    connect(ui->actionSaveSettings, &QAction::triggered, this, &MainWindow::save_settings);
    connect(ui->actionReadMe, &QAction::triggered, this, &MainWindow::action_warning);

    init_stacked_widget();
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
                         "https://github.com/stereomatchingkiss/object_detection_and_alarm/issues"));

    msg_box_->show();
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

QJsonArray MainWindow::dump_stacks_states() const
{
    return {};
}

QJsonObject MainWindow::dump_settings() const
{
    return {};
}

void MainWindow::load_settings(bool)
{

}

void MainWindow::init_widgets_states(QString const &fname)
{

}

void MainWindow::init_widgets_states(QJsonObject const &jobj)
{

}

void MainWindow::save_settings(bool)
{

}

void MainWindow::save_settings_to_file(QString const &save_at) const
{

}

