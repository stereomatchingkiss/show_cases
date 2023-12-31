#pragma once

#include <QDialog>

namespace Ui {
class dialog_alert_sender_settings;
}

class QJsonObject;

struct config_alert_sender;

class dialog_alert_sender_settings : public QDialog
{
    Q_OBJECT

public:
    explicit dialog_alert_sender_settings(QWidget *parent = nullptr);
    ~dialog_alert_sender_settings();

    config_alert_sender get_config() const;
    QJsonObject get_states() const;

    void set_states(QJsonObject const &val);

signals:
    void button_ok_clicked(config_alert_sender const &val);

private:
    void process_ok_button_cliked(bool);

    Ui::dialog_alert_sender_settings *ui;
};
