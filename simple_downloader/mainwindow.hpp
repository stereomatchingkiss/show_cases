#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <network/download_supervisor.hpp>

#include <QMainWindow>
#include <QString>

#include <set>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class widget_download_imgs_from_flist;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_comboBoxFunctions_activated(int index);

private:
    void check_image_existence(QString const &url);
    void download(QString const &url);
    void download_finished(std::shared_ptr<ocv::net::download_supervisor::download_task> task);
    void download_progress(std::shared_ptr<ocv::net::download_supervisor::download_task> task, qint64 bytesReceived, qint64 bytesTotal);

    void next_download();

    Ui::MainWindow *ui;

    QWidget *widget_;
};
#endif // MAINWINDOW_HPP
