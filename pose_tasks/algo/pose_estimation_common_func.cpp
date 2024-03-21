#include "pose_estimation_common_func.hpp"

#include <QJsonArray>
#include <QJsonObject>

QJsonObject convert_to_json(std::vector<flt::cvt::pose::keypoint> const &input)
{
    QJsonArray pts;
    for(auto const &val : input){
        QJsonObject obj;
        obj["x"] = val.x_;
        obj["y"] = val.y_;
        obj["hx"] = val.hx_;
        obj["hy"] = val.hy_;
        obj["score"] = val.score_;
        pts.push_back(obj);
    }

    QJsonObject jobj;
    jobj["pts"] = pts;
    return jobj;
}

std::string pose_model_root()
{
#ifndef WASM_BUILD
    return "assets/";
#else
    return "";
#endif
}

std::string pose_bin_path()
{
    return pose_model_root() + "thunder.bin";
}

std::string pose_param_path()
{
    return pose_model_root() + "thunder.param";
}
