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

    void switch_to_task_selection_page();

    QJsonObject get_states();

    void set_states(QJsonObject const &val);

private slots:
    void on_pushButtonNext_clicked();

private:    
    void init_stacked_widget();
    template<typename T>
    void setup_stack_widget(T *widget, QString const &state_key)
    {        
        update_stack_widget(widget);
        connect(widget,
                &T::switch_to_task_selection_page,
                this,
                &widget_stacks_manager::switch_to_task_selection_page);
        if(stacks_states_.contains(state_key)){
            widget->set_states(stacks_states_[state_key].toObject());
        }else{
            widget->set_states(stacks_states_[state_key].toObject());
        }
    }
    void setup_stacks();
    void update_stack_widget(QWidget *widget);    

    Ui::widget_stacks_manager *ui;

    QWidget *widget_stacks_;
    widget_tasks_selection *widget_tasks_selection_;

    QJsonObject stacks_states_;
};
