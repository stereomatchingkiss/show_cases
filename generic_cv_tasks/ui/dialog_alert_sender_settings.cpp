#include "dialog_alert_sender_settings.hpp"
#include "ui_dialog_alert_sender_settings.h"

#include "../config/config_alert_sender.hpp"

#include <network/websocket_client_controller.hpp>

#include <QJsonObject>

#include <QPushButton>

namespace{

QString const state_websocket_activate("state_websocket_activate");
QString const state_websocket_save_reports("state_websocket_save_reports");
QString const state_websocket_send_by_text("state_websocket_send_by_text");
QString const state_websocket_url("state_websocket_url");

}

dialog_alert_sender_settings::dialog_alert_sender_settings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::dialog_alert_sender_settings)
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
}

dialog_alert_sender_settings::~dialog_alert_sender_settings()
{
    delete ui;
}

config_alert_sender dialog_alert_sender_settings::get_config() const
{
    config_alert_sender config;
    config.activate_ = ui->groupBoxSendAlertTo->isChecked();
    config.save_reports_ = ui->checkBoxSaveReports->isChecked();
    config.send_by_text_ = ui->checkBoxSendByText->isChecked();
    config.url_ = ui->lineEditWebsocketUrl->text();

    return config;
}

QJsonObject dialog_alert_sender_settings::get_states() const
{
    QJsonObject obj;
    obj[state_websocket_activate] = ui->groupBoxSendAlertTo->isChecked();
    obj[state_websocket_save_reports] = ui->checkBoxSaveReports->isChecked();
    obj[state_websocket_send_by_text] = ui->checkBoxSendByText->isChecked();
    obj[state_websocket_url] = ui->lineEditWebsocketUrl->text();

    return obj;
}

void dialog_alert_sender_settings::set_states(const QJsonObject &val)
{
    if(val.contains(state_websocket_activate)){
        ui->groupBoxSendAlertTo->setChecked(val[state_websocket_activate].toBool());
    }
    if(val.contains(state_websocket_save_reports)){
        ui->checkBoxSaveReports->setChecked(val[state_websocket_save_reports].toBool());
    }
    if(val.contains(state_websocket_send_by_text)){
        ui->checkBoxSendByText->setChecked(val[state_websocket_send_by_text].toBool());
    }
    if(val.contains(state_websocket_url)){
        ui->lineEditWebsocketUrl->setText(val[state_websocket_url].toString());
    }
}

void dialog_alert_sender_settings::process_ok_button_cliked(bool)
{
    close();
    emit button_ok_clicked(get_config());
}
