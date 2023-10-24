#pragma once

#include <cv_algo/utils/common_obj_det_type.hpp>

#include <memory>

namespace ocv{

namespace byte_track{

class STrack;
struct Object;

}

byte_track::Object box_info_to_byte_track_obj(utils::box_info const &info);

std::vector<byte_track::Object> box_info_to_byte_track_obj(std::vector<utils::box_info> const &inputs);

utils::box_info byte_track_obj_to_box_info(std::shared_ptr<byte_track::STrack> const &ptr, int label);

std::vector<utils::box_info> byte_track_obj_to_box_info(std::vector<std::shared_ptr<byte_track::STrack>> const &obj, int label);

}
