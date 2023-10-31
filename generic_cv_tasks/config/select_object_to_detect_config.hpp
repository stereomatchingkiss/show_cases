#pragma once

#include <QJsonObject>
#include <QRectF>
#include <QStringList>

#include <set>

struct select_object_to_detect_config
{
    QJsonObject get_states() const;

    void set_states(QJsonObject const &val);

    std::set<std::string> selected_object_;
};
