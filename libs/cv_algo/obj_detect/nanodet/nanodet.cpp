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

std::vector<box_info> nanodet::predict(cv::Mat const &bgr,
                                       float score_threshold,
                                       float nms_threshold,
                                       bool swap_channel)
{
    object_rect effect_roi;
    cv::Mat resized_img;
    resize_uniform(bgr, resized_img, effect_roi, net_->get_input_size(), net_->get_input_size());

    auto boxes_info =
        net_->predict_with_resize_image(resized_img.data, resized_img.cols, resized_img.rows, score_threshold, nms_threshold, swap_channel);

    scale_bbox(bgr.cols, bgr.rows, boxes_info, effect_roi);

    return boxes_info;
}

}
