#ifndef GLOBAL_KEYWORDS_HPP
#define GLOBAL_KEYWORDS_HPP

#include <QString>

#include <string>
#include <vector>

class global_keywords
{
public:
    QString action_classify_alert_path() const;
    QString fall_down_obj_det_alert_path() const;
    QString const tracker_alert_path() const;

    QString cam_config_path() const;
    QString const cam_name() const;
    std::vector<std::string> coco_names() const;
    QString const coco_names_path() const;    
    QString const generic_cv_tasks_assets() const;    

    QString state_cam_states() const;
    QString state_roi() const;
    QString state_stacks_manager() const;
    QString state_tracker_alert() const;
    QString state_version() const;
    QString state_widget_alert_settings() const;
    QString state_widget_object_detect_model_select() const;
    QString state_widget_select_object_to_detect() const;
    QString state_widget_source_selection() const;
};

#endif // GLOBAL_KEYWORDS_HPP
