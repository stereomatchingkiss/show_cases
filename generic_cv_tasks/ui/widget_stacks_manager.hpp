#pragma once

#include <QWidget>

namespace Ui {
class widget_stacks_manager;
}

class QJsonObject;
class QMessageBox;

class widget_object_detect_model_select;
class widget_select_object_to_detect;
class widget_source_selection;
class widget_stream_player;
class widget_tracker_alert;

struct config_alert_sender;

namespace flt::mm{

class single_frame_with_multi_worker_base;

}

namespace flt::ui{

class label_select_roi;

}

class widget_stacks_manager : public QWidget
{
    Q_OBJECT

public:
    explicit widget_stacks_manager(QWidget *parent = nullptr);
    ~widget_stacks_manager();

    QJsonObject get_states() const;

    void set_states(QJsonObject const &val);

private slots:
    void on_pushButtonPrev_clicked();

    void on_pushButtonNext_clicked();

private:
    void init_stacked_widget();

    void create_frame_capture();
    void create_roi_select_stream();

    void next_page_is_label_select_roi();
    void next_page_is_widget_stream_player();
    void next_page_is_widget_select_object_to_detect();
    void next_page_is_widget_source_selection();
    void next_page_is_widget_tracker_alert();    

    void send_alert_by_binary(QByteArray const &msg);
    void send_alert_by_text(QString const &msg);

    void update_position();

    Ui::widget_stacks_manager *ui;

    widget_object_detect_model_select *widget_object_detect_model_select_;
    widget_select_object_to_detect *widget_select_object_to_detect_;
    widget_source_selection *widget_source_selection_;
    widget_stream_player *widget_stream_player_;
    widget_tracker_alert *widget_tracker_alert_;

    flt::ui::label_select_roi *label_select_roi_;
    QMessageBox *msg_box_;
    QTimer *timer_;

    std::unique_ptr<flt::mm::single_frame_with_multi_worker_base> sfwmw_;    
};
