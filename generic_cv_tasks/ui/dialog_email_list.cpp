#include "dialog_email_list.hpp"
#include "ui_dialog_email_list.h"

#include "../config/config_dialog_email_list.hpp"

#include <QJsonArray>
#include <QJsonObject>

namespace{

inline QString state_email_address(){ return "state_email_address"; }
inline QString state_recipient_name(){ return "state_recipient_name"; }

inline QString state_version(){ return "state_version"; }

}

dialog_email_list::dialog_email_list(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::dialog_email_list)
{
    ui->setupUi(this);
}

dialog_email_list::~dialog_email_list()
{
    delete ui;
}

config_dialog_email_list dialog_email_list::get_config() const
{
    config_dialog_email_list config;
    for(int i = 0; i != ui->tableWidget->rowCount(); ++i){
        config.email_address_.push_back(ui->tableWidget->item(i, 0)->text());
        config.recipient_name_.push_back(ui->tableWidget->item(i, 1)->text());
    }

    return config;
}

QJsonObject dialog_email_list::get_states() const
{
    QJsonArray email_addr;
    QJsonArray recipient_name;
    for(int i = 0; i != ui->tableWidget->rowCount(); ++i){
        email_addr.push_back(ui->tableWidget->item(i, 0)->text());
        recipient_name.push_back(ui->tableWidget->item(i, 1)->text());
    }

    QJsonObject obj;
    obj[state_email_address()] = email_addr;
    obj[state_recipient_name()] = recipient_name;
    obj[state_version()] = "1.0";

    return obj;
}

void dialog_email_list::set_states(QJsonObject const &val)
{
    if(val.contains(state_email_address()) && val.contains(state_recipient_name())){
        auto const email_addr = val[state_email_address()].toArray();
        auto const recipient_name = val[state_recipient_name()].toArray();
        ui->tableWidget->setRowCount(email_addr.size());
        for(int i = 0; i != ui->tableWidget->rowCount(); ++i){
            ui->tableWidget->setItem(i, 0, new QTableWidgetItem(email_addr[i].toString()));
            ui->tableWidget->setItem(i, 1, new QTableWidgetItem(recipient_name[i].toString()));
        }
    }
}

void dialog_email_list::on_buttonBox_accepted()
{

}

void dialog_email_list::on_pushButtonAdd_clicked()
{
    ui->tableWidget->setRowCount(ui->tableWidget->rowCount() + 1);
}

void dialog_email_list::on_pushButtonDelete_clicked()
{

}

