#include "face_detector_retina_face_ncnn.hpp"

#include "face_detector_anchor_creator.hpp"

#include "../../ncnn/ncnn_model_loader.hpp"

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

    std::vector<size_t> indexs(boxs.size());
    std::iota(std::begin(indexs), std::end(indexs), 0);
    std::ranges::sort(boxs, std::greater<anchor>());

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
        res[i].final_box_.width = std::clamp(res[i].final_box_.width - res[i].final_box_.x, 0.f, static_cast<float>(target_size - 1));
        res[i].final_box_.height = std::clamp(res[i].final_box_.height - res[i].final_box_.y, 0.f, static_cast<float>(target_size - 1));
    }
}

}

struct face_detector_retina_face_ncnn::impl
{
    impl(std::string const &param, std::string const &bin, float nms_threshold, bool swap_rgb) :
        mloder_(param, bin, &net_),
        nms_threshold_{nms_threshold},
        swap_rgb_{swap_rgb}
    {        
        net_.opt.use_winograd_convolution = true;
        net_.opt.use_sgemm_convolution = true;

        ac_.resize(feat_stride_fpn_.size());
        for(size_t i = 0; i < feat_stride_fpn_.size(); i++){
            int const stride = feat_stride_fpn_[i];
            ac_[i].init(stride, anchor_config_[i]);
        }

        for(auto const &val : feat_stride_fpn_){
            cls_name_.emplace_back("face_rpn_cls_prob_reshape_stride" + std::to_string(val));
            reg_name_.emplace_back("face_rpn_bbox_pred_stride" + std::to_string(val));
            pts_name_.emplace_back("face_rpn_landmark_pred_stride" + std::to_string(val));
        }
    }    

    std::vector<face_detector_box> predict(cv::Mat const &bgr)
    {
        ncnn::Mat input = swap_rgb_ ? ncnn::Mat::from_pixels(bgr.data, ncnn::Mat::PIXEL_BGR2RGB, bgr.cols, bgr.rows) :
                              ncnn::Mat::from_pixels(bgr.data, ncnn::Mat::PIXEL_RGB, bgr.cols, bgr.rows);
        ncnn::Extractor ex = net_.create_extractor();

        ex.input(mloder_.get_input_name().c_str(), input);

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
    nc::ncnn_model_loader mloder_;

    std::vector<face_detector_anchor_creator> ac_;
    std::vector<anchor_cfg> const anchor_config_ = {anchor_cfg(std::vector<float>{32,16}, std::vector<float>{1}, 16),
                                                    anchor_cfg(std::vector<float>{8,4}, std::vector<float>{1}, 16),
                                                    anchor_cfg(std::vector<float>{2,1}, std::vector<float>{1}, 16)};    

    std::vector<std::string> cls_name_;
    std::vector<std::string> reg_name_;
    std::vector<std::string> pts_name_;

    std::vector<int> const feat_stride_fpn_ = {32,16,8};
    float const nms_threshold_;
    bool swap_rgb_;
    static int constexpr target_size_ = 300;    
};

face_detector_retina_face_ncnn::face_detector_retina_face_ncnn(std::string const &param,
                                                                 std::string const &bin,
                                                                 float nms_threshold,
                                                                 bool swap_rgb) :
    face_detector_base(),
    impl_{std::make_unique<impl>(param, bin, nms_threshold, swap_rgb)}
{

}

face_detector_retina_face_ncnn::~face_detector_retina_face_ncnn()
{

}

int face_detector_retina_face_ncnn::get_load_model_state() const noexcept
{
    return impl_->mloder_.get_load_model_state();
}

std::vector<face_detector_box> face_detector_retina_face_ncnn::predict(const cv::Mat &bgr)
{
    return impl_->predict(bgr);
}

}
