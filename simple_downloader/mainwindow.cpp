#include "mainwindow.hpp"
#include "./ui_mainwindow.h"

#include "widget_download_imgs_from_flist.hpp"
#include "widget_gdrive_downloader.hpp"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    widget_ = new widget_download_imgs_from_flist;
    ui->stackedWidget->addWidget(widget_);

    ui->comboBoxFunctions->addItem(tr("Download imgs from file list"));
    ui->comboBoxFunctions->addItem(tr("Download file from gdrive"));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_comboBoxFunctions_activated(int index)
{
    qDebug()<<"current box index = "<<index;
    if(current_index_ != index){
        qDebug()<<"switch widget = "<<index;
        ui->stackedWidget->removeWidget(widget_);
        widget_->deleteLater();
        widget_ = nullptr;

        if(index == 0){
            widget_ = new widget_download_imgs_from_flist;
            ui->stackedWidget->addWidget(widget_);
        }else if(index == 1){
            widget_ = new widget_gdrive_downloader;
            ui->stackedWidget->addWidget(widget_);
        }

        if(widget_){
            ui->stackedWidget->setCurrentWidget(widget_);
        }

        current_index_ = index;
    }
}

