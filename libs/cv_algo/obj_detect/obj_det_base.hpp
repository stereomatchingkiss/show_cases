#pragma once

#include <cv_algo/obj_detect/common_obj_det_type.hpp>

#include <opencv2/core/core.hpp>
#include <net.h>

namespace flt::cvt::det{

class obj_det_base
{
public:
    obj_det_base();
    virtual ~obj_det_base();

    virtual std::vector<box_info> predict(cv::Mat const &rgb,
                                          float score_threshold,
                                          float nms_threshold,
                                          int rotation_angle,
                                          bool hflip) = 0;

};

}
