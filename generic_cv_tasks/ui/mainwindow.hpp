#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>

#include <memory>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class widget_object_detect_model_select;
class widget_select_object_to_detect;
class widget_source_selection;
class widget_stream_player;
class widget_tracker_alert;

namespace flt::mm{

class frame_capture_websocket;
class single_frame_with_multi_worker_base;

}

namespace flt::ui{

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
    void create_frame_capture();
    void create_roi_select_stream();

    void init_stacked_widget();

    void next_page_is_label_select_roi();
    void next_page_is_widget_stream_player();
    void next_page_is_widget_select_object_to_detect();
    void next_page_is_widget_source_selection();
    void next_page_is_widget_tracker_alert();

    Ui::MainWindow *ui;

    flt::ui::label_select_roi *label_select_roi_;
    widget_object_detect_model_select *widget_object_detect_model_select_;
    widget_select_object_to_detect *widget_select_object_to_detect_;
    widget_source_selection *widget_source_selection_;
    widget_stream_player *widget_stream_player_;
    widget_tracker_alert *widget_tracker_alert_;

    std::unique_ptr<flt::mm::single_frame_with_multi_worker_base> sfwmw_;
    std::unique_ptr<flt::mm::frame_capture_websocket> web_frame_;
};
#endif // MAINWINDOW_HPP
