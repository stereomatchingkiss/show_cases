#include "global_keywords.hpp"

#include <utils/file_reader.hpp>

const QString global_keywords::face_recognition_tasks_assets() const
{
    return "FaceRecognitionTasksAssets";
}

const QString global_keywords::mode() const
{
    return "mode";
}

const QString global_keywords::img() const
{
    return "img";
}

QString global_keywords::state_cam_states() const
{
    return "state_cam_states";
}

const QString global_keywords::assets_path() const
{
#ifdef WASM_BUILD
    return "";
#else
    return "assets";
#endif
}

QString global_keywords::cam_config_path() const
{
    return face_recognition_tasks_assets() + "/cam_config/";
}

const QString global_keywords::cam_name() const
{
    return "cam_name";
}

const std::string global_keywords::moble_face_v2_bin() const
{
    std::string const path = "mbv2facenet.bin";

#ifdef WASM_BUILD
    return "" + path;
#else
    return "assets/" + path;
#endif
}

const std::string global_keywords::moble_face_v2_param() const
{
    std::string const path = "mbv2facenet.param";

#ifdef WASM_BUILD
    return "" + path;
#else
    return "assets/" + path;
#endif
}

const std::string global_keywords::retina_face_bin() const
{
    std::string const path = "mnet.25-opt.bin";

#ifdef WASM_BUILD
    return "" + path;
#else
    return "assets/" + path;
#endif
}

const std::string global_keywords::retina_face_param() const
{
    std::string const path = "mnet.25-opt.param";

#ifdef WASM_BUILD
    return "" + path;
#else
    return "assets/" + path;
#endif
}

const QString global_keywords::faces_info() const
{
    return "faces_info";
}

const QString global_keywords::face_similar_score() const
{
    return "face_similar_score";
}

const QString global_keywords::face_features() const
{
    return "face_features";
}

const QString global_keywords::face_image() const
{
    return "face_image";
}

const QString global_keywords::face_name() const
{
    return "face_name";
}

QString global_keywords::state_stacks_manager() const
{
    return "state_stacks_manager";
}

QString global_keywords::state_version() const
{
    return "state_version";
}
