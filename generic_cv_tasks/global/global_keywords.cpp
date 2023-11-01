#include "global_keywords.hpp"

#include <utils/file_reader.hpp>

const QString global_keywords::generic_cv_tasks_assets() const
{
    return "GenericCVTasksAssets";
}

const QString global_keywords::coco_names_path() const
{
    return "assets/obj_detect/coco.names";
}

std::vector<std::string> global_keywords::coco_names() const
{
    return flt::read_file_per_lines("assets/obj_detect/coco.names");
}

QString global_keywords::state_roi() const
{
    return "state_roi";
}

QString global_keywords::state_widget_object_detect_model_select() const
{
    return "state_widget_object_detect_model_select";
}

QString global_keywords::state_widget_select_object_to_detect() const
{
    return "state_widget_select_object_to_detect";
}

QString global_keywords::state_widget_source_selection() const
{
    return "state_widget_source_selection";
}
