#pragma once

#include <QWidget>

class QJsonObject;

namespace Ui {
class widget_action_classify_alert;
}

struct config_action_classify_alert;

class widget_action_classify_alert : public QWidget
{
    Q_OBJECT

public:
    explicit widget_action_classify_alert(QWidget *parent = nullptr);
    ~widget_action_classify_alert();

    config_action_classify_alert get_config() const;
    QJsonObject get_states() const;

    void set_states(QJsonObject const &val);

private:
    Ui::widget_action_classify_alert *ui;
};
