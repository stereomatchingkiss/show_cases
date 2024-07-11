#pragma once

#include <QWidget>

namespace Ui {
class widget_fall_down_obj_det_alert;
}

struct config_fall_down_obj_det_alert;

class widget_fall_down_obj_det_alert : public QWidget
{
    Q_OBJECT

public:
    explicit widget_fall_down_obj_det_alert(QWidget *parent = nullptr);
    ~widget_fall_down_obj_det_alert();

    config_fall_down_obj_det_alert get_config() const;
    QJsonObject get_states() const;

    void set_states(QJsonObject const &val);

private:
    int get_warning_condition() const;

    Ui::widget_fall_down_obj_det_alert *ui;
};
