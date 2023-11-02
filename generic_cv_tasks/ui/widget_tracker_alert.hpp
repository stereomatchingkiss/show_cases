#pragma once

#include <QWidget>

namespace Ui {
class widget_tracker_alert;
}

class QJsonObject;

struct config_tracker_alert;

class widget_tracker_alert : public QWidget
{
    Q_OBJECT

public:
    explicit widget_tracker_alert(QWidget *parent = nullptr);
    ~widget_tracker_alert();

    config_tracker_alert get_config() const noexcept;

    QJsonObject get_states() const;
    void set_states(QJsonObject const &val);

private:
    Ui::widget_tracker_alert *ui;
};
