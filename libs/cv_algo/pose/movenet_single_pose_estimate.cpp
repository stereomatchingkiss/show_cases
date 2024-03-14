#include "movenet_single_pose_estimate.hpp"

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

namespace flt::cvt::pose{

namespace{

template<class ForwardIterator>
inline size_t argmax(ForwardIterator first, ForwardIterator last) {
    return std::distance(first, std::max_element(first, last));
}

}

struct movenet_single_pose_estimate::pimpl
{
    pimpl(std::string const &param,
          std::string const &bin,
          int target_size,
          bool swap_rgb) :
        target_size_{target_size},
        swap_rgb_{swap_rgb}
    {
        net_.load_param(param.c_str());
        net_.load_model(bin.c_str());

        if(target_size == 192){
            feature_size_ = 48;
            kpt_scale_ = 0.02083333395421505;
        }else{
            feature_size_ = 64;
            kpt_scale_ = 0.015625;
        }
        for(int i = 0; i < feature_size_; i++){
            std::vector<float> x, y;
            for (int j = 0; j < feature_size_; j++)
            {
                x.push_back(static_cast<float>(j));
                y.push_back(static_cast<float>(i));
            }
            dist_y_.push_back(y);
            dist_x_.push_back(x);
        }
    }    

    std::tuple<ncnn::Mat, int, int, float> preprocess(cv::Mat const &bgr)
    {
        int w = bgr.cols;
        int h = bgr.rows;
        float scale = 1.f;
        if(w > h){
            scale = (float)target_size_ / w;
            w = target_size_;
            h = static_cast<int>(h * scale);
        }else{
            scale = (float)target_size_ / h;
            h = target_size_;
            w = static_cast<int>(w * scale);
        }

        ncnn::Mat in = ncnn::Mat::from_pixels_resize(bgr.data, swap_rgb_ ? ncnn::Mat::PIXEL_BGR2RGB : ncnn::Mat::PIXEL_RGB,
                                                     bgr.cols, bgr.rows, w, h);
        int const wpad = target_size_ - w;
        int const hpad = target_size_ - h;
        ncnn::Mat in_pad;
        ncnn::copy_make_border(in, in_pad, hpad / 2, hpad - hpad / 2, wpad / 2, wpad - wpad / 2, ncnn::BORDER_CONSTANT, 0.f);

        in_pad.substract_mean_normalize(mean_vals, norm_vals_);

        return {std::move(in_pad), wpad, hpad, scale};
    }

    std::vector<keypoint> predict(cv::Mat const &bgr)
    {
        auto [in_pad, wpad, hpad, scale] = preprocess(bgr);
        ncnn::Extractor ex = net_.create_extractor();
        ex.input("input", in_pad);

        ncnn::Mat regress, center, heatmap, offset;
        ex.extract("regress", regress);
        ex.extract("offset", offset);
        ex.extract("heatmap", heatmap);
        ex.extract("center", center);

        float *center_data = static_cast<float*>(center.data);
        float *heatmap_data = static_cast<float*>(heatmap.data);
        float *offset_data = static_cast<float*>(offset.data);

        int top_index = static_cast<int>(argmax(center_data, center_data+center.h));
        int const ct_y = (top_index / feature_size_);
        int const ct_x = top_index - ct_y * feature_size_;

        int constexpr num_joints = 17;
        std::vector<float> y_regress(num_joints), x_regress(num_joints);
        float *regress_data = static_cast<float*>(regress.channel(ct_y).row(ct_x));
        for(size_t i = 0; i < num_joints; ++i){
            y_regress[i] = regress_data[i] + (float)ct_y;
            x_regress[i] = regress_data[i + num_joints] + (float)ct_x;
        }

        ncnn::Mat kpt_scores = ncnn::Mat(feature_size_ * feature_size_, num_joints, sizeof(float));
        float* scores_data = static_cast<float*>(kpt_scores.data);
        for(int i = 0; i < feature_size_; ++i){
            for (int j = 0; j < feature_size_; ++j){
                std::vector<float> score;
                for (int c = 0; c < num_joints; ++c)
                {
                    float y = (dist_y_[i][j] - y_regress[c]) * (dist_y_[i][j] - y_regress[c]);
                    float x = (dist_x_[i][j] - x_regress[c]) * (dist_x_[i][j] - x_regress[c]);
                    float dist_weight = std::sqrt(y + x) + 1.8f;
                    scores_data[c* feature_size_ * feature_size_ +i* feature_size_ +j] = heatmap_data[i * feature_size_ * num_joints + j * num_joints + c] / dist_weight;
                }
            }
        }
        std::vector<int> kpts_ys, kpts_xs;
        for(int i = 0; i < num_joints; ++i){
            top_index = int(argmax(scores_data + feature_size_ * feature_size_ *i, scores_data + feature_size_ * feature_size_ *(i+1)));
            int top_y = (top_index / feature_size_);
            int top_x = top_index - top_y * feature_size_;
            kpts_ys.push_back(top_y);
            kpts_xs.push_back(top_x);
        }

        std::vector<keypoint> points;
        for(int i = 0; i < num_joints; ++i){
            float kpt_offset_x = offset_data[kpts_ys[i] * feature_size_ * num_joints*2 + kpts_xs[i] * num_joints * 2 + i * 2];
            float kpt_offset_y = offset_data[kpts_ys[i] * feature_size_ * num_joints * 2 + kpts_xs[i] * num_joints * 2 + i * 2+1];

            float x = (kpts_xs[i] + kpt_offset_y) * kpt_scale_ * target_size_;
            float y = (kpts_ys[i] + kpt_offset_x) * kpt_scale_ * target_size_;

            keypoint kpt;
            kpt.x_ = static_cast<int>((x - (wpad / 2)) / scale);
            kpt.y_ = static_cast<int>((y - (hpad / 2)) / scale);
            kpt.score_ = heatmap_data[kpts_ys[i] * feature_size_ * num_joints + kpts_xs[i] * num_joints + i];
            points.push_back(kpt);
        }

        return points;
    }

    ncnn::Net net_;
    int feature_size_;
    float kpt_scale_;
    int target_size_;
    float mean_vals[3] = {127.5f, 127.5f,  127.5f};
    float norm_vals_[3] = {1/ 127.5f, 1 / 127.5f, 1 / 127.5f};
    std::vector<std::vector<float>> dist_y_, dist_x_;
    bool swap_rgb_;
};

movenet_single_pose_estimate::movenet_single_pose_estimate(const std::string &param,
                                                           const std::string &bin,
                                                           int target_size,
                                                           bool swap_rgb) :
    pimpl_(std::make_unique<pimpl>(param, bin, target_size, swap_rgb))
{

}

movenet_single_pose_estimate::~movenet_single_pose_estimate()
{

}

std::vector<keypoint> movenet_single_pose_estimate::predict(cv::Mat const &bgr)
{
    return pimpl_->predict(bgr);
}

}
