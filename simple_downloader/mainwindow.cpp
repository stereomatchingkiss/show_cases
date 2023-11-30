#include "mainwindow.hpp"
#include "./ui_mainwindow.h"

#include "widget_download_imgs_from_flist.hpp"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    widget_ = new widget_download_imgs_from_flist;
    ui->stackedWidget->addWidget(widget_);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_comboBoxFunctions_activated(int index)
{
    if(ui->comboBoxFunctions->currentIndex() != index){
        ui->stackedWidget->removeWidget(widget_);
        widget_->deleteLater();

        if(index == 0){
            widget_ = new widget_download_imgs_from_flist;
            ui->stackedWidget->addWidget(widget_);
        }
    }
}

