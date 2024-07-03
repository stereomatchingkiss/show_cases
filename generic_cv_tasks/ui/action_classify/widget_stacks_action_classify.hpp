#ifndef WIDGET_STACKS_ACTION_CLASSIFY_HPP
#define WIDGET_STACKS_ACTION_CLASSIFY_HPP

#include <QWidget>

namespace Ui {
class widget_stacks_action_classify;
}

namespace flt::mm{

class single_frame_with_multi_worker_base;

}

class widget_action_classify_alert;
class widget_action_classify_model_select;
class widget_roi_selection;
class widget_select_action_to_classify;
class widget_source_selection;
class widget_stream_player;

class QMessageBox;
class QTimer;

class widget_stacks_action_classify : public QWidget
{
    Q_OBJECT

public:
    explicit widget_stacks_action_classify(QWidget *parent = nullptr);
    ~widget_stacks_action_classify();

    QJsonObject get_states() const;

    void set_states(QJsonObject const &val);

signals:
    void switch_to_task_selection_page();

private slots:
    void on_pushButtonPrev_clicked();

    void on_pushButtonNext_clicked();

private:
    void create_frame_capture();
    void create_roi_select_stream();
    void init_stacked_widget();
    void next_page_is_label_select_roi();
    void next_page_is_widget_stream_player();

    void send_alert_by_binary(QByteArray const &msg);
    void send_alert_by_text(QString const &msg);

    void update_position();

    Ui::widget_stacks_action_classify *ui;

    widget_action_classify_alert *widget_action_classify_alert_;
    widget_action_classify_model_select *widget_action_classify_model_select_;
    widget_roi_selection *widget_roi_selection_;
    widget_select_action_to_classify *widget_select_action_to_classify_;
    widget_source_selection *widget_source_selection_;
    widget_stream_player *widget_stream_player_;

    std::unique_ptr<flt::mm::single_frame_with_multi_worker_base> sfwmw_;

    QMessageBox *msg_box_;
    QTimer *timer_;
};

#endif // WIDGET_STACKS_ACTION_CLASSIFY_HPP
