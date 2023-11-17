#ifndef WIDGET_SOURCE_SELECTION_HPP
#define WIDGET_SOURCE_SELECTION_HPP

#include <QWidget>

namespace Ui {
class widget_source_selection;
}

struct config_source_selection;

class widget_source_selection : public QWidget
{
    Q_OBJECT

public:
    explicit widget_source_selection(QWidget *parent = nullptr);
    ~widget_source_selection();

    config_source_selection get_config() const;
    QJsonObject get_states() const;
    void set_states(QJsonObject const &val);

private:
    Ui::widget_source_selection *ui;
};

#endif // WIDGET_SOURCE_SELECTION_HPP
