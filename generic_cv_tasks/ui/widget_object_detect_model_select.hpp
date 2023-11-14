#pragma once

#include <QWidget>

class Ui_widget_object_detect_model_select;

struct config_object_detect_model_select;

class widget_object_detect_model_select : public QWidget
{
    Q_OBJECT

public:
    explicit widget_object_detect_model_select(QWidget *parent = nullptr);
    ~widget_object_detect_model_select();

    config_object_detect_model_select get_config() const;
    QJsonObject get_states() const;

    void set_states(QJsonObject const &val);

private:
    Ui_widget_object_detect_model_select *ui;
};
