#include "widget_source_selection.hpp"
#include "ui_widget_source_selection.h"

#include "../config/config_source_selection.hpp"

#include <multimedia/network/frame_capture_websocket_params.hpp>
#include <multimedia/stream_enum.hpp>

#include <QJsonObject>

using namespace flt::mm;

using stype = stream_source_type;

namespace{

QString const state_source_type("state_source_type");
QString const state_websocket_url("state_websocket_url");

}

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
    if(ui->radioButtonImage->isChecked()){
        config.source_type_ = stype::image;
    }else if(ui->radioButtonWebSocket->isChecked()){
        config.source_type_ = stype::websocket;
    }

    config.websocket_url_ = ui->lineEditWebSockets->text();

    return config;
}

stream_source_type widget_source_selection::get_source_type() const noexcept
{    
    if(ui->radioButtonImage->isChecked()){
        return stype::image;
    }

    return stype::websocket;
}

QJsonObject widget_source_selection::get_states() const
{    
    QJsonObject states;
    states[state_source_type] = static_cast<int>(get_source_type());
    states[state_websocket_url] = ui->lineEditWebSockets->text();

    return states;
}

frame_capture_websocket_params widget_source_selection::get_frame_capture_websocket_params() const
{
    frame_capture_websocket_params params;
    params.url_ = ui->lineEditWebSockets->text();

    return params;
}

void widget_source_selection::set_states(QJsonObject const &val)
{
    if(val.contains(state_websocket_url)){
        ui->lineEditWebSockets->setText(val[state_websocket_url].toString());
    }

    if(val.contains(state_source_type)){
        switch(static_cast<stype>(val[state_source_type].toInt())){
        case stype::image:{
            ui->radioButtonImage->setChecked(true);
            break;
        }
        default:{
            ui->radioButtonWebSocket->setChecked(true);
            break;
        }
        }
    }
}
