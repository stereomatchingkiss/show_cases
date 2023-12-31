#include "widget_download_imgs_from_flist.hpp"
#include "ui_widget_download_imgs_from_flist.h"

#include <QDebug>
#include <QDir>
#include <QFile>
#include <QImageReader>
#include <QNetworkRequest>
#include <QRandomGenerator>
#include <QSettings>
#include <QTextStream>

using namespace ocv::net;

namespace{

QString const key_save_at("key_save_at");
QString const key_save_error_log_at("key_save_error_log_at");
QString const key_url_list("key_url_list");

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

widget_download_imgs_from_flist::widget_download_imgs_from_flist(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::widget_download_imgs_from_flist),
    error_count_{0},
    total_count_{0},
    file_{nullptr}
{
    ui->setupUi(this);

    manager_ = new download_supervisor(this);
    connect(manager_, &download_supervisor::download_finished, this, &widget_download_imgs_from_flist::download_finished);
    connect(manager_, &download_supervisor::download_progress, this, &widget_download_imgs_from_flist::download_progress);

    ui->progressBar->setRange(0, 0);

    QSettings settings;
    if(settings.contains(key_save_at)){
        ui->lineEditSaveAt->setText(settings.value(key_save_at).toString());
    }
    if(settings.contains(key_save_error_log_at)){
        ui->lineEditSaveErroLogAt->setText(settings.value(key_save_error_log_at).toString());
    }
    if(settings.contains(key_url_list)){
        ui->lineEditUrlList->setText(settings.value(key_url_list).toString());
    }
}

widget_download_imgs_from_flist::~widget_download_imgs_from_flist()
{
    QSettings settings;
    settings.setValue(key_save_at, ui->lineEditSaveAt->text());
    settings.setValue(key_save_error_log_at, ui->lineEditSaveErroLogAt->text());
    settings.setValue(key_url_list, ui->lineEditUrlList->text());

    delete ui;
}

void widget_download_imgs_from_flist::on_pushButtonDownload_clicked()
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

void widget_download_imgs_from_flist::check_image_existence(const QString &url)
{
    auto const split_url = url.split("/");
    auto const fname = "test_data/" + split_url[split_url.size() - 1];
    if(QFile::exists(fname)){
        if(QImageReader(fname).canRead()){
            next_download();
        }else{
            QFile::remove(fname);
            download(url);
        }
    }else{
        download(url);
    }
}

void widget_download_imgs_from_flist::download(const QString &url)
{
    auto const img_link = urls_.begin()->simplified().remove("\"");
    qDebug()<<"download img link = "<<img_link;
    QNetworkRequest const request = create_img_download_request(img_link);
    auto const unique_id = manager_->append(request, ui->lineEditSaveAt->text(), 1000 * 60);
    QTimer::singleShot(QRandomGenerator::global()->bounded(1000) + 500,
                       [this, unique_id](){manager_->start_download_task(unique_id);});
}

void widget_download_imgs_from_flist::download_finished(std::shared_ptr<ocv::net::download_supervisor::download_task> task)
{
    if(task->get_network_error_code() != QNetworkReply::NoError){
        QTextStream stream(file_);
        stream<<task->get_url().toString()<<","<<task->get_error_string()<<"\n";
        qDebug()<<task->get_url()<<" download done, error = "<<task->get_error_string();
        QFile::remove(task->get_save_as());
    }
    ui->progressBar->setValue(ui->progressBar->value() + 1);
    next_download();
}

void widget_download_imgs_from_flist::download_progress(std::shared_ptr<ocv::net::download_supervisor::download_task> task,
                                                        qint64 bytesReceived,
                                                        qint64 bytesTotal)
{
    qDebug()<<task->get_url()<<" progress is "<<bytesReceived<<"/"<<bytesTotal;
}

void widget_download_imgs_from_flist::next_download()
{
    if(!urls_.empty()){
        auto const url = *urls_.begin();
        urls_.erase(url);
        ui->labelProgress->setText(QString("Progress left : %1").arg(urls_.size()));
        download(url);
        //check_image_existence(url);
    }
}

