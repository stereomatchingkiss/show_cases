#include "global_keywords.hpp"

const QString global_keywords::generic_cv_tasks_assets() const
{
    return "GenericCVTasksAssets";
}

const QString global_keywords::label_select_roi_path() const
{
    return generic_cv_tasks_assets() + "/label_select_roi.json";
}
