#include "dialog_display_details.hpp"
#include "ui_dialog_display_details.h"

#include "../config/config_dialog_display_details.hpp"

#include <QJsonObject>

namespace{

QString const state_show_confidence("state_show_confidence");
QString const state_show_location("state_show_location");

}

dialog_display_details::dialog_display_details(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::dialog_display_details)
{
    ui->setupUi(this);
}

dialog_display_details::~dialog_display_details()
{
    delete ui;
}

config_dialog_display_details dialog_display_details::get_config() const
{
    config_dialog_display_details config;
    config.show_confidence_ = ui->checkBoxShowConfidence->isChecked();
    config.show_location_ = ui->checkBoxShowLocation->isChecked();

    return config;
}

QJsonObject dialog_display_details::get_states() const
{
    QJsonObject obj;
    obj[state_show_confidence] = ui->checkBoxShowConfidence->isChecked();
    obj[state_show_location] = ui->checkBoxShowLocation->isChecked();

    return obj;
}

void dialog_display_details::set_states(const QJsonObject &val)
{
    if(val.contains(state_show_confidence)){
        ui->checkBoxShowConfidence->setChecked(val[state_show_confidence].toBool());
    }
    if(val.contains(state_show_location)){
        ui->checkBoxShowLocation->setChecked(val[state_show_location].toBool());
    }
}
