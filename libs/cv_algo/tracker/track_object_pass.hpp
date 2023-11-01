#pragma once

#include "cv_algo/obj_detect/common_obj_det_type.hpp"

#include <memory>

namespace flt::cvt::tracker
{

struct track_results;

class track_object_pass
{
public:
    explicit track_object_pass(cv::Rect const &scaled_roi, size_t lost_threshold);
    ~track_object_pass();

    track_results track(std::vector<cvt::det::box_info> const &input);

private:
    struct impl;
    std::unique_ptr<impl> impl_;
};

}
