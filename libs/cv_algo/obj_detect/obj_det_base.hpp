#pragma once

#include <cv_algo/obj_detect/common_obj_det_type.hpp>

#include <opencv2/core/core.hpp>
#include <net.h>

namespace ocv::det{

class obj_det_base
{
public:
    obj_det_base();
    virtual ~obj_det_base();

    virtual std::vector<box_info> predict(cv::Mat const &rgb,
                                          float score_threshold = 0.4f,
                                          float nms_threshold = 0.5f,
                                          int rotation_angle = 0,
                                          bool hflip = false) = 0;

};

}
