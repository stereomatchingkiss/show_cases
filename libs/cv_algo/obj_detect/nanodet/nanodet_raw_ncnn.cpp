#include "nanodet_raw_ncnn.hpp"

#include "../common_obj_det_type.hpp"

#include <algorithm>
#include <concepts>
#include <iostream>
#include <vector>

namespace ocv{

namespace{

inline
    float fast_exp(float x)
{
    union {
        uint32_t i;
        float f;
    } v{};
    v.i = (1 << 23) * (1.4426950409 * x + 126.93490512f);
    return v.f;
}

int activation_function_softmax(const std::floating_point auto* src, std::floating_point auto* dst, int length)
{
    const auto alpha = *std::max_element(src, src + length);
    std::decay_t<decltype(alpha)> denominator{ 0 };

    for (int i = 0; i < length; ++i) {
        dst[i] = fast_exp(src[i] - alpha);
        denominator += dst[i];
    }

    for (int i = 0; i < length; ++i) {
        dst[i] /= denominator;
    }

    return 0;
}

void generate_grid_center_priors(int input_height,
                                 int input_width,
                                 std::vector<int> const &strides,
                                 std::vector<center_prior>& center_priors)
{
    for(int i = 0; i < static_cast<int>(strides.size()); i++){
        int const stride = strides[i];
        int const feat_w = static_cast<int>(std::ceil(static_cast<float>(input_width) / stride));
        int const feat_h = static_cast<int>(std::ceil(static_cast<float>(input_height) / stride));
        for(int y = 0; y < feat_h; ++y){
            for(int x = 0; x < feat_w; ++x){
                center_prior ct;
                ct.x = x;
                ct.y = y;
                ct.stride = stride;
                center_priors.push_back(ct);
            }
        }
    }
}

}

nanodet_raw_ncnn::nanodet_raw_ncnn(const char *param, const char *bin, int num_class, bool use_gpu, int input_size, int max_thread) :
    input_size_{input_size, input_size},
    max_thread_{max_thread},
    num_class_(num_class)
{
    net_ = std::make_unique<ncnn::Net>();
#if NCNN_VULKAN
    has_gpu_ = ncnn::get_gpu_count() > 0 && use_gpu;
#endif
    std::cout<<"has gpu = "<<has_gpu_<<", use gpu = "<<use_gpu<<std::endl;
    net_->opt.use_vulkan_compute = has_gpu_;
    net_->opt.use_fp16_arithmetic = true;
    net_->opt.num_threads = max_thread_;
    load_param_success_ = net_->load_param(param);
    load_model_success_ = net_->load_model(bin);
}

int nanodet_raw_ncnn::get_input_size() const
{
    return input_size_[0];
}

bool nanodet_raw_ncnn::get_load_model_success() const noexcept
{
    return load_model_success_ == 0;
}

bool nanodet_raw_ncnn::get_load_param_success() const noexcept
{
    return load_param_success_ == 0;
}

int nanodet_raw_ncnn::get_load_model_state() const noexcept
{
    if(get_load_model_success() && get_load_param_success()){
        return 0;
    }
    if(get_load_model_success() == true && get_load_param_success() == false){
        return 1;
    }
    if(get_load_model_success() == false && get_load_param_success() == true){
        return 2;
    }
    if(get_load_model_success() == false && get_load_param_success() == false){
        return 3;
    }

    return -1;
}

std::vector<box_info> nanodet_raw_ncnn::predict_with_resize_image(unsigned char *buffer,
                                                                  int width,
                                                                  int height,
                                                                  float score_threshold,
                                                                  float nms_threshold)
{
    ncnn::Mat input;
    preprocess(buffer, width, height, input);

    auto ex = net_->create_extractor();
    ex.input("data", input);

    std::vector<std::vector<box_info>> results;
    results.resize(this->num_class_);

    ncnn::Mat out;
    ex.extract("output", out);

    // generate center priors in format of (x, y, stride)
    std::vector<center_prior> center_priors;
    generate_grid_center_priors(input_size_[0], input_size_[1], strides_, center_priors);

    decode_infer(out, center_priors, score_threshold, results);

    std::vector<box_info> dets;
    for(size_t i = 0; i < results.size(); i++){
        nms(results[i], nms_threshold);
        for(auto box : results[i]){
            dets.push_back(box);
        }
    }

    return dets;
}

void nanodet_raw_ncnn::scale_bbox(int src_w, int src_h, std::vector<box_info> &bboxes, const object_rect &effect_roi)
{
    float const width_ratio = static_cast<float>(src_w) / static_cast<float>(effect_roi.width_);
    float const height_ratio = static_cast<float>(src_h) / static_cast<float>(effect_roi.height_);
    for(size_t i = 0; i < bboxes.size(); ++i){
        box_info &bbox = bboxes[i];
        bbox.x1_ = (bbox.x1_ - effect_roi.x_) * width_ratio;
        bbox.y1_ = (bbox.y1_ - effect_roi.y_) * height_ratio;
        bbox.x2_ = (bbox.x2_ - effect_roi.x_) * width_ratio;
        bbox.y2_ = (bbox.y2_ - effect_roi.y_) * height_ratio;
    }
}

ncnn::Extractor nanodet_raw_ncnn::create_extractor() const
{
    return net_->create_extractor();
}

void nanodet_raw_ncnn::decode_infer(ncnn::Mat &feats,
                                    std::vector<center_prior> &center_priors,
                                    float threshold,
                                    std::vector<std::vector<box_info>> &results)
{
    size_t const num_points = center_priors.size();
    for(int idx = 0; idx < num_points; idx++){
        const int ct_x = center_priors[idx].x;
        const int ct_y = center_priors[idx].y;
        const int stride = center_priors[idx].stride;

        const float* scores = feats.row(idx);
        float score = 0;
        int cur_label = 0;
        for (int label = 0; label < num_class_; label++){
            if (scores[label] > score){
                score = scores[label];
                cur_label = label;
            }
        }
        if (score > threshold){
            const float* bbox_pred = feats.row(idx) + num_class_;
            results[cur_label].push_back(dis_pred_to_box(bbox_pred, cur_label, score, ct_x, ct_y, stride));
        }
    }
}

box_info nanodet_raw_ncnn::dis_pred_to_box(const float *&dfl_det, int label, float score, int x, int y, int stride) const
{
    float const ct_x = static_cast<float>(x * stride);
    float const ct_y = static_cast<float>(y * stride);
    std::vector<float> dis_pred;
    dis_pred.resize(4);
    std::vector<float> dis_after_sm(reg_max_ + 1);
    for(int i = 0; i < 4; ++i){
        float dis = 0;
        activation_function_softmax(dfl_det + i * (reg_max_ + 1), &dis_after_sm[0], reg_max_ + 1);
        for (int j = 0; j < reg_max_ + 1; ++j){
            dis += j * dis_after_sm[j];
        }
        dis *= stride;
        dis_pred[i] = dis;
    }
    float const xmin = std::max(ct_x - dis_pred[0], .0f);
    float const ymin = std::max(ct_y - dis_pred[1], .0f);
    float const xmax = std::min(ct_x + dis_pred[2], static_cast<float>(input_size_[0]));
    float const ymax = std::min(ct_y + dis_pred[3], static_cast<float>(input_size_[1]));

    return box_info {xmin, ymin, xmax, ymax, score, label};
}

void nanodet_raw_ncnn::nms(std::vector<box_info> &input_boxes, float nms_threshold)
{
    std::sort(input_boxes.begin(), input_boxes.end(), [](box_info a, box_info b) { return a.score_ > b.score_; });
    std::vector<float> vArea(input_boxes.size());
    for(int i = 0; i < int(input_boxes.size()); ++i){
        vArea[i] = (input_boxes.at(i).x2_ - input_boxes.at(i).x1_ + 1)
                   * (input_boxes.at(i).y2_ - input_boxes.at(i).y1_ + 1);
    }
    for(int i = 0; i < int(input_boxes.size()); ++i){
        for(int j = i + 1; j < int(input_boxes.size());){
            float const xx1 = (std::max)(input_boxes[i].x1_, input_boxes[j].x1_);
            float const yy1 = (std::max)(input_boxes[i].y1_, input_boxes[j].y1_);
            float const xx2 = (std::min)(input_boxes[i].x2_, input_boxes[j].x2_);
            float const yy2 = (std::min)(input_boxes[i].y2_, input_boxes[j].y2_);
            float const w = (std::max)(float(0), xx2 - xx1 + 1);
            float const h = (std::max)(float(0), yy2 - yy1 + 1);
            float const inter = w * h;
            float const ovr = inter / (vArea[i] + vArea[j] - inter);
            if(ovr >= nms_threshold){
                input_boxes.erase(input_boxes.begin() + j);
                vArea.erase(vArea.begin() + j);
            }
            else{
                ++j;
            }
        }
    }
}

void nanodet_raw_ncnn::preprocess(unsigned char *buffer, int width, int height, ncnn::Mat &in) const
{
    //this model expect bgr image
    in = ncnn::Mat::from_pixels(buffer, ncnn::Mat::PIXEL_BGR, width, height);
    float constexpr mean_vals[3] = { 103.53f, 116.28f, 123.675f };
    float constexpr norm_vals[3] = { 0.017429f, 0.017507f, 0.017125f };
    in.substract_mean_normalize(mean_vals, norm_vals);
}

}
