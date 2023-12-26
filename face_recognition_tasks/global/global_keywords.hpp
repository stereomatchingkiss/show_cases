#ifndef GLOBAL_KEYWORDS_HPP
#define GLOBAL_KEYWORDS_HPP

#include <QString>

#include <string>
#include <vector>

class global_keywords
{
public:
    QString const assets_path() const;
    QString cam_config_path() const;
    QString const cam_name() const;

    std::string const moble_face_v2_bin() const;
    std::string const moble_face_v2_param() const;
    std::string const retina_face_bin() const;
    std::string const retina_face_param() const;

    QString const face_recognition_tasks_assets() const;

    QString state_cam_states() const;    
    QString state_stacks_manager() const;        
    QString state_version() const;    
};

#endif // GLOBAL_KEYWORDS_HPP
