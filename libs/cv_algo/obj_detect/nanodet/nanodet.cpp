//
// Create by RangiLyu
// 2020 / 10 / 2
//

#include "nanodet.hpp"

#include "nanodet_raw_ncnn.hpp"

#include "../common_obj_det_type.hpp"
#include "../obj_det_utils.hpp"
#include "../../utils/image_process.hpp"

#include <opencv2/imgproc.hpp>

#include <algorithm>
#include <iostream>

namespace flt::cvt::det{

nanodet::nanodet(const char* param, const char* bin, int num_class, bool use_gpu, int input_size, int max_thread) :
    obj_det_base(),
    net_(std::make_unique<nanodet_raw_ncnn>(param, bin, num_class, use_gpu, input_size, max_thread))
{        
}

nanodet::~nanodet()
{

}

bool nanodet::get_load_model_success() const noexcept
{
    return net_->get_load_model_success();
}

bool nanodet::get_load_param_success() const noexcept
{
    return net_->get_load_param_success();
}

int nanodet::get_load_model_state() const noexcept
{
    return net_->get_load_model_state();
}

std::vector<box_info> nanodet::predict_with_resize_image(cv::Mat const &bgr,
                                                         float score_threshold,
                                                         float nms_threshold,
                                                         int rotation_angle,
                                                         bool hflip)
{
    adjust_img_orientation(bgr, rotation_angle, hflip);
    return net_->predict_with_resize_image(bgr.data, bgr.cols, bgr.rows, score_threshold, nms_threshold);
}

std::vector<box_info> nanodet::predict(cv::Mat const &bgr,
                                       float score_threshold,
                                       float nms_threshold,
                                       int rotation_angle,
                                       bool hflip)
{
    object_rect effect_roi;
    resize_uniform(bgr, resized_img_, effect_roi, net_->get_input_size(), net_->get_input_size());

    auto boxes_info = predict_with_resize_image(resized_img_, score_threshold, nms_threshold, rotation_angle, hflip);

    auto const abs_rotate_angle = std::abs(rotation_angle);
    bool const should_swap = abs_rotate_angle == 90 || abs_rotate_angle == 270;
    if(should_swap){
        std::swap(effect_roi.height_, effect_roi.width_);
        std::swap(effect_roi.y_, effect_roi.x_);
        scale_bbox(bgr.rows, bgr.cols, boxes_info, effect_roi);
    }else{
        scale_bbox(bgr.cols, bgr.rows, boxes_info, effect_roi);
    }

    return boxes_info;
}

void nanodet::adjust_img_orientation(cv::Mat const &input, int rotate_angle, bool horizontal_flip)
{
    cvt::utils::adjust_img_orientation(input, img_rotate_, rotate_angle, horizontal_flip);
}

}
