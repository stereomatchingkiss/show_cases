#pragma once

#include <cv_algo/obj_detect/common_obj_det_type.hpp>

#include <memory>

namespace flt{

namespace tracker{

class STrack;
struct Object;

}

tracker::Object box_info_to_byte_track_obj(det::box_info const &info);

std::vector<tracker::Object> box_info_to_byte_track_obj(std::vector<det::box_info> const &inputs);

det::box_info byte_track_obj_to_box_info(std::shared_ptr<tracker::STrack> const &ptr, int label);

std::vector<det::box_info> byte_track_obj_to_box_info(std::vector<std::shared_ptr<tracker::STrack>> const &obj, int label);

}
