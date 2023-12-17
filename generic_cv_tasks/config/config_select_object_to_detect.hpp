#pragma once

#include <QJsonObject>
#include <QRectF>
#include <QStringList>

#include <set>

struct config_select_object_to_detect
{
    std::set<std::string> selected_object_;
};
