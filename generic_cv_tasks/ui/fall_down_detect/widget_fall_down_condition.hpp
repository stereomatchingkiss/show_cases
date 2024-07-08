#ifndef WIDGET_FALL_DOWN_PARAM_HPP
#define WIDGET_FALL_DOWN_PARAM_HPP

#include <QWidget>

namespace Ui {
class widget_fall_down_condition;
}

class widget_fall_down_condition : public QWidget
{
    Q_OBJECT

public:
    explicit widget_fall_down_condition(QWidget *parent = nullptr);
    ~widget_fall_down_condition();

    QJsonObject get_states() const;

    void set_states(QJsonObject const &val);

private:
    Ui::widget_fall_down_condition *ui;
};

#endif // WIDGET_FALL_DOWN_PARAM_HPP
