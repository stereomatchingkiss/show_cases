#pragma once

#include <QWidget>

namespace Ui {
class widget_fall_down_obj_det_alert;
}

class widget_fall_down_obj_det_alert : public QWidget
{
    Q_OBJECT

public:
    explicit widget_fall_down_obj_det_alert(QWidget *parent = nullptr);
    ~widget_fall_down_obj_det_alert();

private:
    Ui::widget_fall_down_obj_det_alert *ui;
};
