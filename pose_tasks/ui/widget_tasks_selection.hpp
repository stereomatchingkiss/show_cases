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

signals:
    void task_selected(int index);

private slots:
    void on_comboBoxTasks_activated(int index);

private:
    Ui::widget_tasks_selection *ui;
};

#endif // WIDGET_TASKS_SELECTION_HPP
