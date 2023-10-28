#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>

#include <memory>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class widget_source_selection;
class widget_stream_player;

namespace ocv{

class single_frame_with_multi_worker;

}

namespace ocv::ui{

class label_select_roi;

}

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

    ocv::ui::label_select_roi *label_select_roi_;
    widget_source_selection *widget_source_selection_;
    widget_stream_player *widget_stream_player_;

    std::unique_ptr<ocv::single_frame_with_multi_worker> sfwmw_;
};
#endif // MAINWINDOW_HPP
