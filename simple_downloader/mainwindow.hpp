#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <network/download_supervisor.hpp>

#include <QMainWindow>
#include <QString>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

#include <set>
#include <map>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButtonDownload_clicked();

private:
    void download(QString const &url);
    void download_finished(std::shared_ptr<ocv::net::download_supervisor::download_task> task);
    void download_progress(std::shared_ptr<ocv::net::download_supervisor::download_task> task, qint64 bytesReceived, qint64 bytesTotal);

    void next_download();

    Ui::MainWindow *ui;

    size_t error_count_;
    ocv::net::download_supervisor *manager_;
    size_t total_count_;
    std::set<QString> urls_;

    QFile *file_;
};
#endif // MAINWINDOW_HPP
