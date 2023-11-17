#include "widget_source_selection.hpp"
#include "ui_widget_source_selection.h"

#include "../config/config_source_selection.hpp"

#include <multimedia/stream_enum.hpp>

#include <QJsonObject>

namespace{

QString const state_source_type("state_source_type");
QString const state_websocket_url("state_websocket_url");

}

using namespace flt::mm;

widget_source_selection::widget_source_selection(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::widget_source_selection)
{
    ui->setupUi(this);
}

widget_source_selection::~widget_source_selection()
{
    delete ui;
}

config_source_selection widget_source_selection::get_config() const
{
    config_source_selection config;

    return config;
}

QJsonObject widget_source_selection::get_states() const
{
    using stype = stream_source_type;

    QJsonObject states;
    if(ui->radioButtonImage->isChecked()){
        states[state_source_type] = static_cast<int>(stype::image);
    }else if(ui->radioButtonWebSockets->isChecked()){
        states[state_source_type] = static_cast<int>(stype::websocket);
    }

    states[state_websocket_url] = ui->lineEditWebSockets->text();


    return states;
}

void widget_source_selection::set_states(QJsonObject const &val)
{
    if(val.contains(state_source_type)){
        using stype = stream_source_type;
        switch(static_cast<stype>(val[state_source_type].toInt())){
        case stype::image:{
            ui->radioButtonImage->setChecked(true);
            break;
        }
        case stype::websocket:{
            ui->radioButtonWebSockets->setChecked(true);
            break;
        }
        default:{
            ui->radioButtonWebSockets->setChecked(true);
            break;
        }
        }
    }

    if(val.contains(state_websocket_url)){
        ui->lineEditWebSockets->setText(val[state_websocket_url].toString());
    }
}
