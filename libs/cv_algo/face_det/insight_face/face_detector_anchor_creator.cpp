#include "face_detector_anchor_creator.hpp"

namespace flt::cvt::face{

anchor_cfg::anchor_cfg(std::vector<float> s, std::vector<float> r, int size, float cls_threshold) :
    base_size_{size},
    cls_threshold_{cls_threshold},
    ratios_{std::move(r)},
    scales_{std::move(s)}
{
}

anchor::anchor(){}

void anchor::print()
{
    printf("Rect %f %f %f %f Sorce: %f\n", final_box_.x, final_box_.y, final_box_.width, final_box_.height, score_);
}

size_t face_detector_anchor_creator::init(int stride, anchor_cfg const &cfg)
{
    cret2f const base_anchor(0,0, static_cast<float>(cfg.base_size_ - 1), static_cast<float>(cfg.base_size_ - 1));
    std::vector<cret2f> ratio_anchors;

    cls_threshold_ = cfg.cls_threshold_;

    ratio_enum(base_anchor, cfg.ratios_, ratio_anchors);
    scale_enum(ratio_anchors, cfg.scales_, pre_anchor_);    

    anchor_stride_ = stride;
    anchor_num_ = static_cast<int>(pre_anchor_.size());

    return pre_anchor_.size();
}

void face_detector_anchor_creator::filter_anchor(ncnn::Mat &cls, ncnn::Mat &reg, ncnn::Mat &pts, std::vector<anchor> &proposals) const
{
    int const w = cls.w;
    int const h = cls.h;

    int const pts_length = pts.c / anchor_num_ / 2;

    for(int i = 0; i < h; ++i){
        for(int j = 0; j < w; ++j){
            int const index = i * w + j;
            for(int a = 0; a < anchor_num_;++a){
                // cls.channel(0 | 1) 存放的是两个anchor 分类为不是人脸的概率  cls.channel(2 | 3) 是 分类为人脸的概率
                if(cls.channel(anchor_num_ + a)[index] >= cls_threshold_){
                    //printf("%f\n", cls.channel(anchor_num + a)[index]);
                    cret2f const pre_anchor_real(j * anchor_stride_ + pre_anchor_[a][0],
                                                 i * anchor_stride_ + pre_anchor_[a][1],
                                                 j * anchor_stride_ + pre_anchor_[a][2],
                                                 i * anchor_stride_ + pre_anchor_[a][3]);

                    cret2f const delta(reg.channel(a * 4 + 0)[index],
                                       reg.channel(a * 4 + 1)[index],
                                       reg.channel(a * 4 + 2)[index],
                                       reg.channel(a * 4 + 3)[index]);

                    anchor res;
                    res.anchor_ = cv::Rect_<float>(pre_anchor_real[0], pre_anchor_real[1], pre_anchor_real[2], pre_anchor_real[3]);
                    res.score_ = cls.channel(a + anchor_num_)[index];
                    res.center_ = cv::Point(j, i);

                    box_pred(pre_anchor_real, delta, res.final_box_);

                    std::vector<cv::Point2f> pts_delta(pts_length);
                    for(int p  = 0; p < pts_length; ++p){
                        pts_delta[p].x = pts.channel(a * pts_length * 2 + p * 2)[index];
                        pts_delta[p].y = pts.channel(a * pts_length * 2 + p * 2 + 1)[index];
                    }

                    landmark_pred(pre_anchor_real, pts_delta, res.pts_);

                    proposals.emplace_back(std::move(res));
                }
            }
        }
    }
}

void face_detector_anchor_creator::box_pred(cret2f const &per_anc, cret2f const &delta, cv::Rect_<float> &box) const
{
    float const w = per_anc[2] - per_anc[0] + 1;
    float const h = per_anc[3] - per_anc[1] + 1;
    float const x_center = per_anc[0] + 0.5f * (w - 1);
    float const y_center = per_anc[1] + 0.5f * (h - 1);

    float const x_d  = delta[0];
    float const y_d  = delta[1];
    float const w_d  = delta[2];
    float const h_d  = delta[3];

    float const real_x_center = x_d * w + x_center;
    float const real_y_center = y_d * h + y_center;
    float const real_w = std::exp(w_d) * w;
    float const real_h = std::exp(h_d) * h;

    float const w_transform = 0.5f * (real_w - 1.0f);
    float const h_transform = 0.5f * (real_h - 1.0f);
    box = cv::Rect_<float>(real_x_center - w_transform,
                           real_y_center - h_transform,
                           real_x_center + w_transform,
                           real_y_center + h_transform);
}

void face_detector_anchor_creator::landmark_pred(cret2f const &box,
                                                 std::vector<cv::Point2f> const &pts_delta,
                                                 std::vector<cv::Point2f>& landmark_pre) const
{
    float const w = box[2] - box[0] + 1;
    float const h = box[3] - box[1] + 1;
    float const x_center = box[0] + 0.5f * (w - 1);
    float const y_center = box[1] + 0.5f * (h - 1);

    landmark_pre.resize(pts_delta.size());

    for(size_t i = 0; i < pts_delta.size(); ++i){
        landmark_pre[i].x = pts_delta[i].x * w + x_center;
        landmark_pre[i].y = pts_delta[i].y * h + y_center;
    }
}

void face_detector_anchor_creator::ratio_enum(cret2f const &base_anchor,
                                              std::vector<float> const &ratio,
                                              std::vector<cret2f> &ratio_anchor) const
{
    float const w = base_anchor[2] - base_anchor[0] + 1;
    float const h = base_anchor[3] - base_anchor[1] + 1;
    float const x_center = base_anchor[0] + 0.5f * (w - 1);
    float const y_center = base_anchor[1] + 0.5f * (h - 1);

    ratio_anchor.clear();
    float const orign_size = w * h;
    for(size_t i = 0; i < ratio.size(); i++){
        float const r = ratio[i];
        float const ratio_size = orign_size / r;

        float const w_r = std::sqrt(ratio_size);
        float const h_r = w_r;

        ratio_anchor.push_back(cret2f(x_center - 0.5f * (w_r - 1),
                                      y_center - 0.5f * (h_r - 1),
                                      x_center + 0.5f * (w_r - 1),
                                      y_center + 0.5f * (h_r - 1)));
    }
}

void face_detector_anchor_creator::scale_enum(std::vector<cret2f>const &ratio_anchor,
                                              std::vector<float> const &scales,
                                              std::vector<cret2f> &scale_anchor) const
{
    for(size_t r = 0; r < ratio_anchor.size(); ++r){
        cret2f const rat_an = ratio_anchor[r];
        float const w = rat_an[2] - rat_an[0] + 1;
        float const h = rat_an[3] - rat_an[1] + 1;
        float const x_center = rat_an[0] + 0.5f * (w - 1);
        float const y_center = rat_an[1] + 0.5f * (h - 1);

        for(size_t i = 0; i < scales.size(); ++i)
        {
            float w_s = w * scales[i];
            float h_s = h * scales[i];

            scale_anchor.push_back(cret2f(x_center - 0.5f * (w_s - 1),
                                          y_center - 0.5f * (h_s - 1),
                                          x_center + 0.5f * (w_s - 1),
                                          y_center + 0.5f * (h_s -1 )));
        }
    }
}

}
