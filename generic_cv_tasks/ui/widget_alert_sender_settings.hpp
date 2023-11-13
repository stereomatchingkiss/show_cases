#pragma once

#include <QWidget>

namespace Ui {
class widget_alert_sender_settings;
}

class QJsonObject;

struct config_alert_sender;

class widget_alert_sender_settings : public QWidget
{
    Q_OBJECT

public:
    explicit widget_alert_sender_settings(QWidget *parent = nullptr);
    ~widget_alert_sender_settings();

    config_alert_sender get_config() const;
    QJsonObject get_states() const;

    void set_states(QJsonObject const &val);

signals:
    void button_ok_clicked(config_alert_sender const &val);

private:
    void process_ok_button_cliked(bool);

    Ui::widget_alert_sender_settings *ui;
};
