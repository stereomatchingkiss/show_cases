#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class widget_source_selection;
class widget_stream_player;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButtonNext_clicked();

    void on_pushButtonPrev_clicked();

private:
    Ui::MainWindow *ui;

    widget_source_selection *widget_source_selection_;
    widget_stream_player *widget_stream_player_;
};
#endif // MAINWINDOW_HPP
