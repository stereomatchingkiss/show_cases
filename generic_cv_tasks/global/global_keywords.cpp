#include "global_keywords.hpp"

#include <utils/file_reader.hpp>

const QString global_keywords::generic_cv_tasks_assets() const
{
    return "GenericCVTasksAssets";
}

const QString global_keywords::label_select_roi_path() const
{
    return generic_cv_tasks_assets() + "/label_select_roi.json";
}

const QString global_keywords::coco_names_path() const
{
    return "assets/obj_detect/coco.names";
}

std::vector<std::string> global_keywords::coco_names() const
{
    return ocv::read_file_per_lines("assets/obj_detect/coco.names");
}
