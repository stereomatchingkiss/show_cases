#include "face_detector_retina_face_ncnn.hpp"

#include "face_detector_anchor_creator.hpp"

#include <net.h>

#include <iostream>
#include <map>
#include <numeric>
#include <ranges>

namespace flt::cvt::face{

namespace{

void box_nms_cpu(std::vector<anchor>& boxs, const float threshold, std::vector<anchor>& res, int target_size)
{
    res.clear();
    if(boxs.size() == 0){
        return;
    }

    std::vector<size_t> indexs{0};
    std::sort(std::begin(boxs), std::end(boxs), std::greater<anchor>());

    while(indexs.size() > 0){
        size_t const good_idx = indexs[0];
        res.push_back(boxs[good_idx]);

        std::vector<size_t> tmp = std::move(indexs);
        indexs.clear();
        for(size_t i = 1; i < tmp.size(); ++i){
            size_t const tmp_i = tmp[i];
            float const inter_x1 = std::max(boxs[good_idx][0], boxs[tmp_i][0]);
            float const inter_y1 = std::max(boxs[good_idx][1], boxs[tmp_i][1]);
            float const inter_x2 = std::min(boxs[good_idx][2], boxs[tmp_i][2]);
            float const inter_y2 = std::min(boxs[good_idx][3], boxs[tmp_i][3]);

            float const w = std::max(inter_x2 - inter_x1 + 1, 0.0f);
            float const h = std::max(inter_y2 - inter_y1 + 1, 0.0f);

            float const inter = w * h;

            float const area1 = (boxs[good_idx][2] - boxs[good_idx][0] + 1) * (boxs[good_idx][3] - boxs[good_idx][1] + 1);
            float const area2 = (boxs[tmp_i][2] - boxs[tmp_i][0] + 1) * (boxs[tmp_i][3] - boxs[tmp_i][1] + 1);

            if(float const iou = inter / (area1 + area2 - inter); iou <= threshold){
                indexs.push_back(tmp_i);
            }
        }
    }

    for(size_t i = 0; i < res.size(); ++i){
        res[i].final_box_.x = std::clamp(res[i].final_box_.x, 0.f, static_cast<float>(target_size - 1));
        res[i].final_box_.y = std::clamp(res[i].final_box_.y, 0.f, static_cast<float>(target_size - 1));
        res[i].final_box_.width = std::clamp(res[i].final_box_.width, 0.f, static_cast<float>(target_size - 1));
        res[i].final_box_.height = std::clamp(res[i].final_box_.height, 0.f, static_cast<float>(target_size - 1));
    }
}

}

struct face_detector_retina_face_ncnn::impl
{
    impl(std::string const &param, std::string const &bin, float nms_threshold) :
        nms_threshold_{nms_threshold}
    {
        net_.opt.num_threads = 4;
        net_.opt.use_winograd_convolution = true;
        net_.opt.use_sgemm_convolution = true;

        load_param_success_ = net_.load_param(param.c_str());
        load_model_success_ = net_.load_model(bin.c_str());

        ac_.resize(feat_stride_fpn_.size());
        for(size_t i = 0; i < feat_stride_fpn_.size(); i++){
            int const stride = feat_stride_fpn_[i];
            ac_[i].init(stride, anchor_config_[i]);
        }

        input_name_ = net_.input_names()[0];
        output_name_ = net_.output_names()[net_.output_names().size() - 1];

        for(auto const &val : feat_stride_fpn_){
            cls_name_.emplace_back("face_rpn_cls_prob_reshape_stride" + std::to_string(val));
            reg_name_.emplace_back("face_rpn_bbox_pred_stride" + std::to_string(val));
            pts_name_.emplace_back("face_rpn_landmark_pred_stride" + std::to_string(val));
        }
    }

    bool get_load_model_success() const noexcept
    {
        return load_model_success_ == 0;
    }

    bool get_load_param_success() const noexcept
    {
        return load_param_success_ == 0;
    }

    int get_load_model_state() const noexcept
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

    std::vector<face_detector_box> predict(cv::Mat const &bgr)
    {
        ncnn::Mat input = ncnn::Mat::from_pixels(bgr.data, ncnn::Mat::PIXEL_BGR2RGB, bgr.cols, bgr.rows);        
        ncnn::Extractor ex = net_.create_extractor();

        ex.input(input_name_.c_str(), input);

        std::vector<anchor> proposals;
        for(size_t i = 0; i < feat_stride_fpn_.size(); ++i){
            ncnn::Mat cls;
            ncnn::Mat reg;
            ncnn::Mat pts;

            ex.extract(cls_name_[i].c_str(), cls);
            ex.extract(reg_name_[i].c_str(), reg);
            ex.extract(pts_name_[i].c_str(), pts);            

            ac_[i].filter_anchor(cls, reg, pts, proposals);
        }

        std::vector<anchor> finalres;
        box_nms_cpu(proposals, nms_threshold_, finalres, target_size_);        

        std::vector<face_detector_box> results;
        for(size_t i = 0; i < finalres.size(); ++i){
            face_detector_box box;
            box.confidence_ = finalres[i].score_;
            box.rect_ = finalres[i].final_box_;
            box.landmark_pts_ = std::move(finalres[i].pts_);
            results.emplace_back(std::move(box));
        }

        return results;
    }

    ncnn::Net net_;

    std::vector<face_detector_anchor_creator> ac_;
    std::vector<anchor_cfg> const anchor_config_ = {anchor_cfg(std::vector<float>{32,16}, std::vector<float>{1}, 16),
                                                    anchor_cfg(std::vector<float>{8,4}, std::vector<float>{1}, 16),
                                                    anchor_cfg(std::vector<float>{2,1}, std::vector<float>{1}, 16)};    

    std::vector<std::string> cls_name_;
    std::vector<std::string> reg_name_;
    std::vector<std::string> pts_name_;

    std::vector<int> const feat_stride_fpn_ = {32,16,8};
    float const nms_threshold_;
    static int constexpr target_size_ = 300;

    std::string input_name_;
    std::string output_name_;

    int load_model_success_ = -1;
    int load_param_success_ = -1;
};

face_detector_retina_face_ncnn::face_detector_retina_face_ncnn(std::string const &param,
                                                                 std::string const &bin,
                                                                 float nms_threshold) :
    impl_{std::make_unique<impl>(param, bin, nms_threshold)}
{

}

face_detector_retina_face_ncnn::~face_detector_retina_face_ncnn()
{

}

int face_detector_retina_face_ncnn::get_load_model_state() const noexcept
{
    return impl_->get_load_model_state();
}

std::vector<face_detector_box> face_detector_retina_face_ncnn::predict(const cv::Mat &bgr)
{
    return impl_->predict(bgr);
}

}
