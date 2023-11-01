#include "config_source_selection.hpp"

#include <QJsonObject>

namespace{

QString const state_max_fps("state_max_fps");
QString const state_rtsp_url("state_rtsp_url");
QString const state_source_type("state_source_type");
QString const state_video_url("state_video_url");
QString const state_webcam_index("state_webcam_index");

}

QJsonObject config_source_selection::get_states() const
{
    QJsonObject obj;
    obj[state_max_fps] = max_fps_;
    obj[state_rtsp_url] = rtsp_url_;
    obj[state_source_type] = static_cast<int>(source_type_);
    obj[state_video_url] = video_url_;
    obj[state_webcam_index] = webcam_;

    return obj;
}

void config_source_selection::set_states(const QJsonObject &val)
{
    if(val.contains(state_max_fps)){
        max_fps_ = val[state_max_fps].toInt();
    }
    if(val.contains(state_rtsp_url)){
        rtsp_url_ = val[state_rtsp_url].toString();
    }
    if(val.contains(state_source_type)){
        source_type_ = static_cast<flt::mm::stream_source_type>(val[state_source_type].toInt());
    }
    if(val.contains(state_video_url)){
        video_url_ = val[state_video_url].toString();
    }
    if(val.contains(state_webcam_index)){
        webcam_ = val[state_webcam_index].toString();
    }
}
