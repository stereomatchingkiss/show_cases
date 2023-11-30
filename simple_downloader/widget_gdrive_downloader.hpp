#ifndef GDRIVE_DOWNLOADER_HPP
#define GDRIVE_DOWNLOADER_HPP

#include <network/download_supervisor.hpp>

#include <QWidget>

namespace Ui {
class widget_gdrive_downloader;
}

class widget_gdrive_downloader : public QWidget
{
    Q_OBJECT

public:
    explicit widget_gdrive_downloader(QWidget *parent = nullptr);
    ~widget_gdrive_downloader();

private slots:
    void on_pushButtonDownload_clicked();

private:
    void download(QString const &url);
    void download_finished(std::shared_ptr<ocv::net::download_supervisor::download_task> task);
    void download_progress(std::shared_ptr<ocv::net::download_supervisor::download_task> task, qint64 bytesReceived, qint64 bytesTotal);

    Ui::widget_gdrive_downloader *ui;

    ocv::net::download_supervisor *manager_;
};

#endif // GDRIVE_DOWNLOADER_HPP
