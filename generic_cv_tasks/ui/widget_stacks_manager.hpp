#pragma once

#include <QJsonObject>

#include <QWidget>

namespace Ui {
class widget_stacks_manager;
}

class QJsonObject;

class widget_stacks_object_tracking;
class widget_tasks_selection;

class widget_stacks_manager : public QWidget
{
    Q_OBJECT

public:
    explicit widget_stacks_manager(QWidget *parent = nullptr);
    ~widget_stacks_manager();

    QJsonObject get_states();

    void set_states(QJsonObject const &val);

private slots:
    void on_pushButtonNext_clicked();

private:
    void enable_next_button();
    void init_stacked_widget();
    void setup_stacks();
    void update_stack_widget(QWidget *widget);

    Ui::widget_stacks_manager *ui;

    QWidget *widget_stacks_;
    widget_tasks_selection *widget_tasks_selection_;

    QJsonObject stacks_states_;
};
