#ifndef WIDGET_DOWNLOAD_IMGS_FROM_FLIST_HPP
#define WIDGET_DOWNLOAD_IMGS_FROM_FLIST_HPP

#include <network/download_supervisor.hpp>

#include <QWidget>

#include <set>

namespace Ui {
class widget_download_imgs_from_flist;
}

class widget_download_imgs_from_flist : public QWidget
{
    Q_OBJECT

public:
    explicit widget_download_imgs_from_flist(QWidget *parent = nullptr);
    ~widget_download_imgs_from_flist();

private slots:
    void on_pushButtonDownload_clicked();

private:
    void check_image_existence(QString const &url);
    void download(QString const &url);
    void download_finished(std::shared_ptr<ocv::net::download_supervisor::download_task> task);
    void download_progress(std::shared_ptr<ocv::net::download_supervisor::download_task> task, qint64 bytesReceived, qint64 bytesTotal);

    void next_download();

    Ui::widget_download_imgs_from_flist *ui;

    size_t error_count_;
    ocv::net::download_supervisor *manager_;
    size_t total_count_;
    std::set<QString> urls_;

    QFile *file_;
};

#endif // WIDGET_DOWNLOAD_IMGS_FROM_FLIST_HPP
