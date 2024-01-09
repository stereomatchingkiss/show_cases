#ifndef GLOBAL_KEYWORDS_HPP
#define GLOBAL_KEYWORDS_HPP

#include <QString>

#include <string>
#include <vector>

class global_keywords
{
public:
    std::string anpr_task_assets() const;

    std::string nanodet_320_bin_path() const;
    std::string nanodet_320_param_path() const;
    std::string nanodet_416_bin_path() const;
    std::string nanodet_416_param_path() const;

    std::string paddle_ocr_rec_model_path() const;

    QString state_stacks_manager() const;
    QString state_version() const;    
    QString state_widget_object_detect_model_select() const;    
    QString state_widget_source_selection() const;    
};

#endif // GLOBAL_KEYWORDS_HPP
