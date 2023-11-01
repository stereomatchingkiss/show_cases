#ifndef CONFIG_SOURCE_SELECTION_HPP
#define CONFIG_SOURCE_SELECTION_HPP

#include <multimedia/stream_enum.hpp>

#include <QString>

class QJsonObject;

struct config_source_selection
{
    int max_fps_;
    QString rtsp_url_;
    QString video_url_;
    flt::mm::stream_source_type source_type_;
    QString url_;
    QString webcam_url_;
};

#endif // CONFIG_SOURCE_SELECTION_HPP
