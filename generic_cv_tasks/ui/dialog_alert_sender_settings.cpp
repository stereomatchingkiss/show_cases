#include "dialog_alert_sender_settings.hpp"
#include "ui_dialog_alert_sender_settings.h"

#include "../config/config_alert_sender.hpp"

#include "dialog_email_list.hpp"

#include <network/websocket_client_controller.hpp>

#include <QJsonObject>

#include <QPushButton>

#include <QSettings>

namespace{

inline QString state_email_address(){ return "state_email_address"; }
inline QString state_email_alert_on(){ return "state_email_alert_on"; }
inline QString state_email_list() { return "state_email_list"; }
inline QString state_email_password(){ return "opojmc^&%@#6098"; }

inline QString state_websocket_activate(){ return "state_websocket_activate"; }
inline QString state_websocket_save_reports(){ return "state_websocket_save_reports"; }
inline QString state_websocket_send_by_text(){ return "state_websocket_send_by_text"; }
inline QString state_websocket_url(){ return "state_websocket_url"; }

inline QString state_version(){ return "state_version"; }

}

dialog_alert_sender_settings::dialog_alert_sender_settings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::dialog_alert_sender_settings),
    dialog_email_list_{new dialog_email_list{this}}
{
    ui->setupUi(this);

    connect(ui->buttonBox->button(QDialogButtonBox::Ok), &QPushButton::clicked,
            this, &dialog_alert_sender_settings::process_ok_button_cliked);

    ui->checkBoxSendByText->setToolTip(tr("Binary data transmission offers improved performance, "
                                          "but the server must utilize QWebsocketServer to receive the message. "
                                          "In most instances, text transmission is sufficient."));
#ifdef WASM_BUILD
    ui->checkBoxSaveReports->setVisible(false);
    ui->checkBoxSaveReports->setChecked(false);
#endif

    QSettings settings;
    if(settings.contains(state_email_address())){
        ui->lineEditGmailAddress->setText(settings.value(state_email_address()).toString());
    }
    if(settings.contains(state_email_password())){
        ui->lineEditEmailPassword->setText(settings.value(state_email_password()).toString());
    }
}

dialog_alert_sender_settings::~dialog_alert_sender_settings()
{
    QSettings settings;
    settings.setValue(state_email_address(), ui->lineEditGmailAddress->text());
    settings.setValue(state_email_password(), ui->lineEditEmailPassword->text());

    delete ui;
}

config_alert_sender dialog_alert_sender_settings::get_config() const
{
    config_alert_sender config;
    config.activate_ = ui->groupBoxSendAlertByWebSocket->isChecked();
    config.save_reports_ = ui->checkBoxSaveReports->isChecked();
    config.send_by_text_ = ui->checkBoxSendByText->isChecked();
    config.url_ = ui->lineEditWebsocketUrl->text();

    config.config_dialog_email_list_ = dialog_email_list_->get_config();
    config.email_address_ = ui->lineEditGmailAddress->text();
    config.email_alert_on_ = ui->groupBoxSendAlertByEmail->isChecked();
    config.email_password_ = ui->lineEditEmailPassword->text();

    return config;
}

QJsonObject dialog_alert_sender_settings::get_states() const
{
    QJsonObject obj;

    obj[state_email_alert_on()] = ui->groupBoxSendAlertByEmail->isChecked();
    obj[state_email_list()] = dialog_email_list_->get_states();

    obj[state_websocket_activate()] = ui->groupBoxSendAlertByWebSocket->isChecked();
    obj[state_websocket_save_reports()] = ui->checkBoxSaveReports->isChecked();
    obj[state_websocket_send_by_text()] = ui->checkBoxSendByText->isChecked();
    obj[state_websocket_url()] = ui->lineEditWebsocketUrl->text();

    obj[state_version()] = "1.0";

    return obj;
}

void dialog_alert_sender_settings::set_states(const QJsonObject &val)
{
    if(val.contains(state_email_alert_on())){
        ui->groupBoxSendAlertByEmail->setChecked(val[state_email_alert_on()].toBool());
    }
    if(val.contains(state_email_list())){
        dialog_email_list_->set_states(val[state_email_list()].toObject());
    }

    if(val.contains(state_websocket_activate())){
        ui->groupBoxSendAlertByWebSocket->setChecked(val[state_websocket_activate()].toBool());
    }
    if(val.contains(state_websocket_save_reports())){
        ui->checkBoxSaveReports->setChecked(val[state_websocket_save_reports()].toBool());
    }
    if(val.contains(state_websocket_send_by_text())){
        ui->checkBoxSendByText->setChecked(val[state_websocket_send_by_text()].toBool());
    }
    if(val.contains(state_websocket_url())){
        ui->lineEditWebsocketUrl->setText(val[state_websocket_url()].toString());
    }
}

void dialog_alert_sender_settings::process_ok_button_cliked(bool)
{
    close();
    emit button_ok_clicked(get_config());
}

void dialog_alert_sender_settings::on_pushButtonSendEmailTo_clicked()
{
#ifndef WASM_BUILD
    dialog_email_list_->exec();
#else
    dialog_email_list_->show();
#endif
}

