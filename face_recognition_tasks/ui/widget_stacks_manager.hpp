#ifndef WIDGET_STACKS_MANAGER_HPP
#define WIDGET_STACKS_MANAGER_HPP

#include <QWidget>

namespace Ui {
class widget_stacks_manager;
}

class widget_stacks_faces_search;

class widget_stacks_manager : public QWidget
{
    Q_OBJECT

public:
    explicit widget_stacks_manager(QWidget *parent = nullptr);
    ~widget_stacks_manager();

private:
    void init_stacked_widget();

    Ui::widget_stacks_manager *ui;

    widget_stacks_faces_search *widget_stacks_faces_search_;
};

#endif // WIDGET_STACKS_MANAGER_HPP
