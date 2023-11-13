#include "widget_alert_sender_settings.hpp"
#include "ui_widget_alert_sender_settings.h"

#include "../config/config_alert_sender.hpp"

#include "../global/global_object.hpp"

#include <network/websocket_client_controller.hpp>

#include <QJsonObject>

#include <QPushButton>

namespace{

QString const state_websocket_activate("state_websocket_activate");
QString const state_websocket_send_by_text("state_websocket_send_by_text");
QString const state_websocket_url("state_websocket_url");

}

widget_alert_sender_settings::widget_alert_sender_settings(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::widget_alert_sender_settings)
{
    ui->setupUi(this);

    connect(ui->buttonBox->button(QDialogButtonBox::Ok), &QPushButton::clicked,
            this, &widget_alert_sender_settings::process_ok_button_cliked);

    ui->checkBoxSendByText->setToolTip(tr("Binary data transmission offers improved performance, "
                                          "but the server must utilize QWebsocketServer to receive the message. "
                                          "In most instances, text transmission is sufficient."));
}

widget_alert_sender_settings::~widget_alert_sender_settings()
{
    delete ui;
}

config_alert_sender widget_alert_sender_settings::get_config() const
{
    config_alert_sender config;
    config.activate_ = ui->groupBoxSendAlertTo->isChecked();
    config.send_by_text_ = ui->checkBoxSendByText->isChecked();
    config.url_ = ui->lineEditWebsocketUrl->text();

    return config;
}

QJsonObject widget_alert_sender_settings::get_states() const
{
    QJsonObject obj;
    obj[state_websocket_activate] = ui->groupBoxSendAlertTo->isChecked();
    obj[state_websocket_send_by_text] = ui->checkBoxSendByText->isChecked();
    obj[state_websocket_url] = ui->lineEditWebsocketUrl->text();

    return obj;
}

void widget_alert_sender_settings::set_states(const QJsonObject &val)
{
    if(val.contains(state_websocket_activate)){
        ui->groupBoxSendAlertTo->setChecked(val[state_websocket_activate].toBool());
    }
    if(val.contains(state_websocket_send_by_text)){
        ui->checkBoxSendByText->setChecked(val[state_websocket_send_by_text].toBool());
    }
    if(val.contains(state_websocket_url)){
        ui->lineEditWebsocketUrl->setText(val[state_websocket_url].toString());
    }
}

void widget_alert_sender_settings::process_ok_button_cliked(bool)
{
    close();
    emit get_websocket_controller().reconnect_if_needed(ui->lineEditWebsocketUrl->text());
    emit button_ok_clicked(get_config());
}
