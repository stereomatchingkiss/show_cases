#ifndef WIDGET_TASKS_SELECTION_HPP
#define WIDGET_TASKS_SELECTION_HPP

#include <QWidget>

namespace Ui {
class widget_tasks_selection;
}

struct config_tasks_selection;

class widget_tasks_selection : public QWidget
{
    Q_OBJECT

public:
    explicit widget_tasks_selection(QWidget *parent = nullptr);
    ~widget_tasks_selection();

    config_tasks_selection get_config() const noexcept;
    QJsonObject get_states() const;
    void set_states(QJsonObject const &val);

private:
    void set_task(int task);

    Ui::widget_tasks_selection *ui;
};

#endif // WIDGET_TASKS_SELECTION_HPP
