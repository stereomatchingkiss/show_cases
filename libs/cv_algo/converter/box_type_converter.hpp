#pragma once

#include <cv_algo/obj_detect/common_obj_det_type.hpp>

#include <memory>

namespace ocv{

namespace byte_track{

class STrack;
struct Object;

}

byte_track::Object box_info_to_byte_track_obj(box_info const &info);

std::vector<byte_track::Object> box_info_to_byte_track_obj(std::vector<box_info> const &inputs);

box_info byte_track_obj_to_box_info(std::shared_ptr<byte_track::STrack> const &ptr, int label);

std::vector<box_info> byte_track_obj_to_box_info(std::vector<std::shared_ptr<byte_track::STrack>> const &obj, int label);

}
