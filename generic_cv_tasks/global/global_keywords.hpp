#ifndef GLOBAL_KEYWORDS_HPP
#define GLOBAL_KEYWORDS_HPP

#include <QString>

#include <string>
#include <vector>

class global_keywords
{
public:
    QString const generic_cv_tasks_assets() const;    
    QString const coco_names_path() const;

    std::vector<std::string> coco_names() const;

    QString state_roi() const;
    QString state_widget_object_detect_model_select() const;
    QString state_widget_select_object_to_detect() const;
    QString state_widget_source_selection() const;
};

#endif // GLOBAL_KEYWORDS_HPP
