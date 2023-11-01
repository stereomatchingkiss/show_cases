#pragma once

#include <cv_algo/obj_detect/common_obj_det_type.hpp>

#include <memory>

namespace flt::cvt{

namespace tracker{

class STrack;
struct Object;

}

cvt::tracker::Object box_info_to_byte_track_obj(cvt::det::box_info const &info);

std::vector<cvt::tracker::Object> box_info_to_byte_track_obj(std::vector<cvt::det::box_info> const &inputs);

cvt::det::box_info byte_track_obj_to_box_info(std::shared_ptr<cvt::tracker::STrack> const &ptr);

std::vector<cvt::det::box_info> byte_track_obj_to_box_info(std::vector<std::shared_ptr<cvt::tracker::STrack>> const &obj);

}
