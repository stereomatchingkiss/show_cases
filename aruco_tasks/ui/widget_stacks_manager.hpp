#ifndef WIDGET_STACKS_MANAGER_HPP
#define WIDGET_STACKS_MANAGER_HPP

#include <QWidget>

namespace Ui {
class widget_stacks_manager;
}

class widget_stacks_aruco_creator;
class widget_tasks_selection;

class widget_stacks_manager : public QWidget
{
    Q_OBJECT

public:
    explicit widget_stacks_manager(QWidget *parent = nullptr);
    ~widget_stacks_manager();

private:
    void back_to_task_selection();
    void init_stacked_widget();
    void switch_task(int index);

    Ui::widget_stacks_manager *ui;

    widget_stacks_aruco_creator *widget_stacks_aruco_creator_;
    widget_tasks_selection *widget_tasks_selection_;
};

#endif // WIDGET_STACKS_MANAGER_HPP
