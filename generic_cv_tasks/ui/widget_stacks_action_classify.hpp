#ifndef WIDGET_STACKS_ACTION_CLASSIFY_HPP
#define WIDGET_STACKS_ACTION_CLASSIFY_HPP

#include <QWidget>

namespace Ui {
class widget_stacks_action_classify;
}

class widget_action_classify_model_select;
class widget_select_action_to_classify;
class widget_source_selection;
class widget_stream_player;

class widget_stacks_action_classify : public QWidget
{
    Q_OBJECT

public:
    explicit widget_stacks_action_classify(QWidget *parent = nullptr);
    ~widget_stacks_action_classify();

    QJsonObject get_states() const;

    void set_states(QJsonObject const &val);

signals:
    void enable_next_button();

private slots:
    void on_pushButtonPrev_clicked();

    void on_pushButtonNext_clicked();

private:
    void init_stacked_widget();

    Ui::widget_stacks_action_classify *ui;

    widget_action_classify_model_select *widget_action_classify_model_select_;
    widget_select_action_to_classify *widget_select_action_to_classify_;
    widget_source_selection *widget_source_selection_;
    widget_stream_player *widget_stream_player_;
};

#endif // WIDGET_STACKS_ACTION_CLASSIFY_HPP
