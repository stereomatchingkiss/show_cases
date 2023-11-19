#include "config_read_write.hpp"

#include "../global/global_keywords.hpp"

#include <json/json_utils.hpp>

#include <QJsonDocument>

void config_read_write::set_widget_source_selection(QJsonObject val)
{
    widget_source_selection_ = std::move(val);
}

void config_read_write::set_widget_stream_player(QJsonObject val)
{
    widget_stream_player_ = std::move(val);
}

QJsonObject config_read_write::dumps() const
{
    QJsonObject obj;
    global_keywords gk;
    obj[gk.state_widget_source_selection()] = widget_source_selection_;
    obj[gk.state_widget_stream_player()] = widget_stream_player_;

    return obj;
}

QJsonObject config_read_write::read(const QString &val)
{
    return flt::json::parse_file_to_jobj(val);
}

QJsonObject config_read_write::read(const QByteArray &val)
{
    return QJsonDocument::fromJson(val).object();
}

void config_read_write::write(const QString &val)
{
    flt::json::write_file_to_json(dumps(), val);
}

void config_read_write::write(const QJsonObject &obj, const QString &val)
{
    flt::json::write_file_to_json(obj, val);
}
