#pragma once

#include <cv_algo/obj_detect/common_obj_det_type.hpp>

#include <memory>

struct config_generic_obj_detector;

class generic_obj_detector{

public:
    generic_obj_detector(config_generic_obj_detector config);
    ~generic_obj_detector();

    std::vector<flt::cvt::det::box_info> predict(cv::Mat &rgb);

private:
    struct impl;
    std::unique_ptr<impl> impl_;
};
