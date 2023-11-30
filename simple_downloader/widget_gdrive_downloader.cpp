#include "widget_gdrive_downloader.hpp"
#include "ui_widget_gdrive_downloader.h"

#include <QDebug>
#include <QDir>
#include <QFile>
#include <QNetworkRequest>
#include <QUrlQuery>
#include <QSettings>

using namespace ocv::net;

namespace{

QString const key_destination("key_destination");
QString const key_file_id("key_file_id");

QNetworkRequest create_gfile_download_request(const QString &url, QByteArray const &id)
{
    QUrlQuery query;
    query.addQueryItem("alt","media");

    QUrl gurl(QString("https://www.googleapis.com/drive/v3/files/%1").arg(id));
    gurl.setQuery(query.query());

    QNetworkRequest request(gurl);
    //todo : complete the credential requirements of google
    //QString headerData = "Bearer " + m_Settings->get_Google_Drive_Settings
    //                                 (Google_Drive::AccessTokenEnum);
    //request.setRawHeader("Authorization", headerData.toLocal8Bit());

    return request;
}

}

widget_gdrive_downloader::widget_gdrive_downloader(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::widget_gdrive_downloader)
{
    ui->setupUi(this);

    manager_ = new download_supervisor(this);
    connect(manager_, &download_supervisor::download_finished, this, &widget_gdrive_downloader::download_finished);
    connect(manager_, &download_supervisor::download_progress, this, &widget_gdrive_downloader::download_progress);

    QSettings settings;
    if(settings.contains(key_destination)){
        ui->lineEditDestination->setText(settings.value(key_destination).toString());
    }
    if(settings.contains(key_file_id)){
        ui->lineEditFileID->setText(settings.value(key_file_id).toString());
    }
}

widget_gdrive_downloader::~widget_gdrive_downloader()
{
    QSettings settings;
    settings.setValue(key_destination, ui->lineEditDestination->text());
    settings.setValue(key_file_id, ui->lineEditFileID->text());

    delete ui;
}

void widget_gdrive_downloader::download(const QString &url)
{
    qDebug()<<"download gfile link = "<<url;
    QNetworkRequest const request = create_gfile_download_request(url, ui->lineEditFileID->text().toLatin1());
    auto const unique_id = manager_->append(request, "", 1000 * 60);
    manager_->start_download_task(unique_id);
}

void widget_gdrive_downloader::download_finished(std::shared_ptr<ocv::net::download_supervisor::download_task> task)
{
    if(task->get_network_error_code() != QNetworkReply::NoError){
        qDebug()<<task->get_url()<<" download done, error = "<<task->get_error_string();
    }
}

void widget_gdrive_downloader::download_progress(std::shared_ptr<ocv::net::download_supervisor::download_task> task,
                                                 qint64 bytesReceived,
                                                 qint64 bytesTotal)
{
    qDebug()<<task->get_url()<<" progress is "<<bytesReceived<<"/"<<bytesTotal;
}

void widget_gdrive_downloader::on_pushButtonDownload_clicked()
{
    QDir().mkdir(ui->lineEditDestination->text());
    download(QString("https://drive.google.com/u/0/uc?id=%1&export=download").arg(ui->lineEditFileID->text()));
}

