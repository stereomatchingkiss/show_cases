#pragma once

#include <QString>

class QJsonObject;

namespace ocv::json{

QJsonObject parse_file_to_jobj(QString const &fname);

void write_file_to_json(QJsonObject const &obj, QString const &save_as);

}
