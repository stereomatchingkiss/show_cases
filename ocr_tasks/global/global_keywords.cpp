#include "global_keywords.hpp"

#include <utils/file_reader.hpp>

const QString global_keywords::ocr_tasks_assets() const
{
    return "OcrTasksAssets";
}

QString global_keywords::cam_config_path() const
{
    return ocr_tasks_assets() + "/cam_config/";
}

QString global_keywords::state_widget_source_selection() const
{
    return "state_widget_source_selection";
}
