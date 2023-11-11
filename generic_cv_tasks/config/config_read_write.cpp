#include "config_read_write.hpp"

#include "../global/global_keywords.hpp"

#include <json/json_utils.hpp>

void config_read_write::set_roi(QJsonObject val)
{
    roi_ = std::move(val);
}

void config_read_write::set_widget_alert_settings(QJsonObject val)
{
    widget_alert_settings_ = std::move(val);
}

void config_read_write::set_widget_object_detect_model_select(QJsonObject val)
{
    widget_object_detect_model_select_ = std::move(val);
}

void config_read_write::set_widget_select_object_to_detect(QJsonObject val)
{
    widget_select_object_to_detect_ = std::move(val);
}

void config_read_write::set_widget_source_selection(QJsonObject val)
{
    widget_source_selection_ = std::move(val);
}

void config_read_write::set_widget_tracker_alert(QJsonObject val)
{
    widget_tracker_alert_ = std::move(val);
}

QJsonObject config_read_write::read(const QString &val)
{
    return flt::json::parse_file_to_jobj(val);
}

void config_read_write::write(const QString &val)
{
    QJsonObject obj;
    global_keywords gk;
    obj[gk.state_roi()] = roi_;
    obj[gk.state_widget_alert_settings()] = widget_alert_settings_;
    obj[gk.state_widget_object_detect_model_select()] = widget_object_detect_model_select_;
    obj[gk.state_widget_select_object_to_detect()] = widget_select_object_to_detect_;
    obj[gk.state_widget_source_selection()] = widget_source_selection_;
    obj[gk.state_tracker_alert()] = widget_tracker_alert_;

    flt::json::write_file_to_json(obj, val);
}
