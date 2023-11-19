#ifndef GLOBAL_KEYWORDS_HPP
#define GLOBAL_KEYWORDS_HPP

#include <QString>

#include <string>
#include <vector>

class global_keywords
{
public:
    QString cam_config_path() const;
    QString const ocr_tasks_assets() const;

    QString state_widget_source_selection() const;
    QString state_widget_stream_player() const;
};

#endif // GLOBAL_KEYWORDS_HPP
