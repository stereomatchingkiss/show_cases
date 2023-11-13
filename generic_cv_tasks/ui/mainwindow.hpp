#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>

#include <memory>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class widget_alert_sender_settings;

class widget_object_detect_model_select;
class widget_select_object_to_detect;
class widget_source_selection;
class widget_stream_player;
class widget_tracker_alert;

class QMessageBox;
class QTimer;

namespace flt::mm{

class frame_capture_websocket;
class single_frame_with_multi_worker_base;

}

namespace flt::net{

class websocket_client_controller;

}

namespace flt::ui{

class label_select_roi;

}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButtonNext_clicked();

    void on_pushButtonPrev_clicked();

private:
    void action_about_qt(bool);
    void action_contact_me(bool);
    void action_server_call();
    void action_warning(bool);

    void create_frame_capture();
    void create_roi_select_stream();

    void init_stacked_widget();

    void next_page_is_label_select_roi();
    void next_page_is_widget_stream_player();
    void next_page_is_widget_select_object_to_detect();
    void next_page_is_widget_source_selection();
    void next_page_is_widget_tracker_alert();

    QJsonObject dump_settings() const;
    void load_settings(bool);
    void init_widgets_states(QString const &fname);
    void save_settings(bool);
    void save_settings_to_file(QString const &save_at) const;

    void send_alert_by_binary(QByteArray const &msg);
    void send_alert_by_text(QString const &msg);

    void update_position();

    Ui::MainWindow *ui;
    
    widget_alert_sender_settings *widget_alert_sender_settings_;
    flt::ui::label_select_roi *label_select_roi_;
    QMessageBox *msg_box_;
    QTimer *timer_;
    widget_object_detect_model_select *widget_object_detect_model_select_;
    widget_select_object_to_detect *widget_select_object_to_detect_;
    widget_source_selection *widget_source_selection_;
    widget_stream_player *widget_stream_player_;
    widget_tracker_alert *widget_tracker_alert_;   

    std::unique_ptr<flt::mm::single_frame_with_multi_worker_base> sfwmw_;
    std::unique_ptr<flt::mm::frame_capture_websocket> web_frame_;
    std::unique_ptr<flt::net::websocket_client_controller> websocket_;
};
#endif // MAINWINDOW_HPP
