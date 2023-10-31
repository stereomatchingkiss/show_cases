#pragma once

#include <QWidget>

class Ui_widget_object_detect_model_select;

struct object_detect_model_select_config;

class widget_object_detect_model_select : public QWidget
{
    Q_OBJECT

public:
    explicit widget_object_detect_model_select(QWidget *parent = nullptr);
    ~widget_object_detect_model_select();

    object_detect_model_select_config get_config() const;
    QJsonObject get_states() const;

    void set_states(QJsonObject const &val);

private:
    void set_model_index(int idx) noexcept;

    Ui_widget_object_detect_model_select *ui;
};
