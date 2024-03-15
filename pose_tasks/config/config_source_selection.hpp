#pragma once

#include <multimedia/stream_enum.hpp>

#include <QString>

class QJsonObject;

struct config_source_selection
{
    flt::mm::stream_source_type source_type_;
    QString websocket_url_;
};
