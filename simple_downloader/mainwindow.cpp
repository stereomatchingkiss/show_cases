#include "mainwindow.hpp"
#include "./ui_mainwindow.h"

#include <QDebug>
#include <QDir>
#include <QFile>
#include <QNetworkRequest>
#include <QRandomGenerator>
#include <QTextStream>

#include <set>

using namespace ocv::net;

namespace{

QNetworkRequest create_img_download_request(const QString &url)
{
    QNetworkRequest request(url);
    QStringList const agents{"msnbot-media/1.1 (+http://search.msn.com/msnbot.htm)",
                             "Googlebot-Image/1.0",
                             "Mozilla/5.0 (compatible; Bingbot/2.0; +http://www.bing.com/bingbot.htm)",
                             "Mozilla/5.0 (compatible; Googlebot/2.1; +http://www.google.com/bot.html)"};
    request.setHeader(QNetworkRequest::UserAgentHeader, agents[QRandomGenerator::global()->bounded(agents.size())]);

    return request;
}

}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow),
    error_count_{0},
    total_count_{0},
    file_{nullptr}
{
    ui->setupUi(this);

    manager_ = new download_supervisor(this);
    connect(manager_, &download_supervisor::download_finished, this, &MainWindow::download_finished);
    connect(manager_, &download_supervisor::download_progress, this, &MainWindow::download_progress);

    ui->progressBar->setRange(0, 0);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::download(const QString &url)
{
    auto const img_link = urls_.begin()->simplified().remove("\"");
    qDebug()<<"download img link = "<<img_link;
    QNetworkRequest const request = create_img_download_request(img_link);
    auto const unique_id = manager_->append(request, ui->lineEditSaveAt->text(), 1000 * 60);
    QTimer::singleShot(QRandomGenerator::global()->bounded(1000) + 500,
                       [this, unique_id](){manager_->start_download_task(unique_id);});
}

void MainWindow::download_finished(std::shared_ptr<ocv::net::download_supervisor::download_task> task)
{
    //qDebug()<<task->get_url()<<" download done, error = "<<task->get_error_string();
    if(task->get_network_error_code() != QNetworkReply::NoError){
        QTextStream stream(file_);
        stream<<task->get_url().toString()<<","<<task->get_error_string()<<"\n";
        qDebug()<<task->get_url()<<" download done, error = "<<task->get_error_string();
    }
    ui->progressBar->setValue(ui->progressBar->value() + 1);
    next_download();
}

void MainWindow::download_progress(std::shared_ptr<ocv::net::download_supervisor::download_task> task,
                                   qint64 bytesReceived,
                                   qint64 bytesTotal)
{
    qDebug()<<task->get_url()<<" progress is "<<bytesReceived<<"/"<<bytesTotal;
}

void MainWindow::next_download()
{
    if(!urls_.empty()){
        auto const url = *urls_.begin();
        urls_.erase(url);
        download(url);
    }
}

void MainWindow::on_pushButtonDownload_clicked()
{
    QDir().mkdir(ui->lineEditSaveAt->text());
    if(file_){
        file_->close();
        file_->deleteLater();
    }

    file_ = new QFile(ui->lineEditSaveErroLogAt->text(), this);
    file_->open(QIODevice::WriteOnly);

    urls_ = std::set<QString>();
    if(QFile file(ui->lineEditUrlList->text()); file.open(QIODevice::ReadOnly)){
        for(QTextStream in(&file); !in.atEnd();){
            urls_.insert(in.readLine());
        }
        qDebug()<<urls_.size();
    }
    total_count_ = urls_.size();

    ui->progressBar->setRange(0, static_cast<int>(urls_.size()));
    next_download();
}

