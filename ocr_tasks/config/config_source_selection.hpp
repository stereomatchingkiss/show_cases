#ifndef CONFIG_SOURCE_SELECTION_HPP
#define CONFIG_SOURCE_SELECTION_HPP

#include <multimedia/stream_enum.hpp>

#include <QString>

class QJsonObject;

struct config_source_selection
{    
    flt::mm::stream_source_type source_type_;    
    QString webcam_url_;
};

#endif // CONFIG_SOURCE_SELECTION_HPP
