#include "config_estimate_many_pose_similarity_worker.hpp"

#include <QJsonDocument>
#include <QJsonObject>

QString config_estimate_many_pose_similarity_worker::generate_json_info() const
{
    QJsonObject obj;
    obj["mode"] = "start";
    obj["data_folder"] = data_folder_;
    obj["im_folder"] = image_folder_;

    return QJsonDocument(obj).toJson();
}
