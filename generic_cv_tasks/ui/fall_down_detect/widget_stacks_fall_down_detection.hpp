#ifndef WIDGET_STACKS_FALL_DOWN_DETECTION_HPP
#define WIDGET_STACKS_FALL_DOWN_DETECTION_HPP

#include <QWidget>

namespace Ui {
class widget_stacks_fall_down_detection;
}

class QMessageBox;

class frame_capture_creator;

class widget_fall_down_condition;
class widget_fall_down_obj_det_alert;
class widget_object_detect_model_select;
class widget_roi_selection;
class widget_source_selection;
class widget_stream_player;

class widget_stacks_fall_down_detection : public QWidget
{
    Q_OBJECT

public:
    explicit widget_stacks_fall_down_detection(QWidget *parent = nullptr);
    ~widget_stacks_fall_down_detection();

    QJsonObject get_states() const;

    void set_states(QJsonObject const &val);

signals:
    void switch_to_task_selection_page();

private slots:
    void on_pushButtonPrev_clicked();

    void on_pushButtonNext_clicked();

private:
    void create_frame_capture();
    void init_stacked_widget();
    void next_page_is_roi_selection();
    void next_page_is_widget_stream_player();
    void send_alert_by_binary(QByteArray const &msg);
    void send_alert_by_text(QString const &msg);
    void update_position();       

    Ui::widget_stacks_fall_down_detection *ui;

    widget_fall_down_condition *widget_fall_down_condition_;
    widget_fall_down_obj_det_alert *widget_fall_down_obj_det_alert_;
    widget_object_detect_model_select *widget_object_detect_model_select_;
    widget_roi_selection *widget_roi_selection_;
    widget_source_selection *widget_source_selection_;
    widget_stream_player *widget_stream_player_;

    frame_capture_creator *fcreator_;

    QMessageBox *msg_box_;    
};

#endif // WIDGET_STACKS_FALL_DOWN_DETECTION_HPP
