//
// Create by RangiLyu
// 2020 / 10 / 2
//

#include "nanodet.hpp"

#include "nanodet_raw_ncnn.hpp"

#include "../common_obj_det_type.hpp"
#include "../../utils/image_process.hpp"

#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include <algorithm>
#include <iostream>

namespace ocv{

nanodet::nanodet(const char* param, const char* bin, int num_class, bool use_gpu, bool swap_rgb, int input_size, int max_thread) :
    net_(std::make_unique<nanodet_raw_ncnn>(param, bin, num_class, use_gpu, input_size, max_thread)),
    swap_rgb_(swap_rgb)
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

std::vector<box_info> nanodet::predict_with_resize_image(cv::Mat image,
                                                         float score_threshold,
                                                         float nms_threshold,
                                                         int rotation_angle,
                                                         bool hflip)
{
    if(swap_rgb_){
        cv::cvtColor(image, input_img_, cv::COLOR_RGB2BGR);
    }else{
        input_img_ = image;
    }

    adjust_img_orientation(input_img_, rotation_angle, hflip);
    return net_->predict_with_resize_image(input_img_.data, input_img_.cols, input_img_.rows, score_threshold, nms_threshold);
}

std::vector<box_info> nanodet::predict(cv::Mat const &image,
                                       float score_threshold,
                                       float nms_threshold,
                                       int rotation_angle,
                                       bool hflip)
{
    object_rect effect_roi;
    utils::resize_uniform(image, resized_img_, effect_roi, net_->get_input_size(), net_->get_input_size());

    auto boxes_info = predict_with_resize_image(resized_img_, score_threshold, nms_threshold, rotation_angle, hflip);

    auto const abs_rotate_angle = std::abs(rotation_angle);
    bool const should_swap = abs_rotate_angle == 90 || abs_rotate_angle == 270;
    if(should_swap){
        std::swap(effect_roi.height_, effect_roi.width_);
        std::swap(effect_roi.y_, effect_roi.x_);
        nanodet_raw_ncnn::scale_bbox(image.rows, image.cols, boxes_info, effect_roi);
    }else{
        nanodet_raw_ncnn::scale_bbox(image.cols, image.rows, boxes_info, effect_roi);
    }

    return boxes_info;
}

void nanodet::adjust_img_orientation(cv::Mat const &input, int rotate_angle, bool horizontal_flip)
{
    utils::adjust_img_orientation(input, img_rotate_, rotate_angle, horizontal_flip);
}

void nanodet::set_swap_rgb(bool val)
{
    swap_rgb_ = val;
}

void nanodet::scale_bbox(int src_w, int src_h, std::vector<box_info> &bboxes, object_rect const &effect_roi)
{
    nanodet_raw_ncnn::scale_bbox(src_w, src_h, bboxes, effect_roi);
}

}
