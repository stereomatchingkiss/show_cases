#ifndef GLOBAL_KEYWORDS_HPP
#define GLOBAL_KEYWORDS_HPP

#include <QString>

#include <string>
#include <vector>

class global_keywords
{
public:
    QString const generic_cv_tasks_assets() const;
    QString const label_select_roi_path() const;
    QString const coco_names_path() const;

    std::vector<std::string> coco_names() const;
};

#endif // GLOBAL_KEYWORDS_HPP
