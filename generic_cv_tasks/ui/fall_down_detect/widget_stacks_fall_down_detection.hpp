#ifndef WIDGET_STACKS_FALL_DOWN_DETECTION_HPP
#define WIDGET_STACKS_FALL_DOWN_DETECTION_HPP

#include <QWidget>

namespace Ui {
class widget_stacks_fall_down_detection;
}

class QMessageBox;

class widget_fall_down_param;
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
    void init_stacked_widget();

    Ui::widget_stacks_fall_down_detection *ui;

    widget_fall_down_param *widget_fall_down_param_;
    widget_object_detect_model_select *widget_object_detect_model_select_;
    widget_roi_selection *widget_roi_selection_;
    widget_source_selection *widget_source_selection_;
    widget_stream_player *widget_stream_player_;       

    QMessageBox *msg_box_;
    QTimer *timer_;
};

#endif // WIDGET_STACKS_FALL_DOWN_DETECTION_HPP
