#include "movenet_single_pose_estimate.hpp"

#include "pose_estimation_results.hpp"

#include <opencv2/imgproc.hpp>

#include <net.h>

namespace flt::cvt::pose{

namespace{

template<class ForwardIterator>
inline static size_t argmax(ForwardIterator first, ForwardIterator last) {
    return std::distance(first, std::max_element(first, last));
}

}

struct movenet_single_pose_estimate::pimpl
{
    pimpl(std::string const &param,
          std::string const &bin,
          int min_valid_keypoints,
          float min_confidence,
          int target_size) :
        target_size_(target_size),
        min_confidence_(min_confidence),
        min_valid_keypoints_(min_valid_keypoints)
    {
        net_.opt.use_vulkan_compute = false;
        net_.opt.use_fp16_arithmetic = true;
        net_.opt.num_threads = 4;
        load_param_success_ = net_.load_param(param.c_str());
        load_model_success_ = net_.load_model(bin.c_str());

        input_name_ = net_.input_names()[0];
        output_name_ = net_.output_names()[net_.output_names().size() - 1];

        if(target_size_ == 192){
            feature_size_ = 48;
            kpt_scale_ = 0.02083333395421505;
        }else{
            feature_size_ = 64;
            kpt_scale_ = 0.015625;
        }
        for (int i = 0; i < feature_size_; i++){
            std::vector<float> x, y;
            for (int j = 0; j < feature_size_; j++){
                x.push_back(j);
                y.push_back(i);
            }
            dist_y_.push_back(std::move(y));
            dist_x_.push_back(std::move(x));
        }
    }

    std::vector<float> forward_input(cv::Mat const &bgr)
    {
        cv::Mat resize_img;
        if(target_size_ > 0){
            cv::resize(bgr, resize_img, cv::Size(target_size_, target_size_));
            cv::cvtColor(resize_img, resize_img, cv::COLOR_BGR2RGB);
        }else{
            cv::cvtColor(bgr, resize_img, cv::COLOR_BGR2RGB);
        }

        resize_img.convertTo(resize_img, CV_32S);

        return {};
    }

    pose_estimation_results detect(cv::Mat const &bgr)
    {
        int w = bgr.cols;
        int h = bgr.rows;
        float scale = 1.f;
        if (w > h)
        {
            scale = (float)target_size_ / w;
            w = target_size_;
            h = h * scale;
        }
        else
        {
            scale = (float)target_size_ / h;
            h = target_size_;
            w = w * scale;
        }

        ncnn::Mat in = ncnn::Mat::from_pixels_resize(bgr.data, ncnn::Mat::PIXEL_RGB, bgr.cols, bgr.rows, w, h);
        int wpad = target_size_ - w;
        int hpad = target_size_ - h;
        ncnn::Mat in_pad;
        ncnn::copy_make_border(in, in_pad, hpad / 2, hpad - hpad / 2, wpad / 2, wpad - wpad / 2, ncnn::BORDER_CONSTANT, 0.f);

        ncnn::Extractor ex = net_.create_extractor();
        in_pad.substract_mean_normalize(mean_vals_.data(), norm_vals_.data());

        ex.input("input", in_pad);

        ncnn::Mat regress, center, heatmap, offset;

        ex.extract("regress", regress);
        ex.extract("offset", offset);
        ex.extract("heatmap", heatmap);
        ex.extract("center", center);

        float* center_data = (float*)center.data;
        float* heatmap_data = (float*)heatmap.data;
        float* offset_data = (float*)offset.data;

        int top_index = 0;
        float top_score = 0;

        top_index = int(argmax(center_data, center_data+center.h));
        top_score = *std::max_element(center_data, center_data + center.h);

        int ct_y = (top_index / feature_size);
        int ct_x = top_index - ct_y * feature_size;

        std::vector<float> y_regress(num_joints), x_regress(num_joints);
        float* regress_data = (float*)regress.channel(ct_y).row(ct_x);
        for (size_t i = 0; i < num_joints; i++)
        {
            y_regress[i] = regress_data[i] + (float)ct_y;
            x_regress[i] = regress_data[i + num_joints] + (float)ct_x;
        }

        ncnn::Mat kpt_scores = ncnn::Mat(feature_size * feature_size, num_joints, sizeof(float));
        float* scores_data = (float*)kpt_scores.data;
        for (int i = 0; i < feature_size; i++)
        {
            for (int j = 0; j < feature_size; j++)
            {
                std::vector<float> score;
                for (int c = 0; c < num_joints; c++)
                {
                    float y = (dist_y[i][j] - y_regress[c]) * (dist_y[i][j] - y_regress[c]);
                    float x = (dist_x[i][j] - x_regress[c]) * (dist_x[i][j] - x_regress[c]);
                    float dist_weight = std::sqrt(y + x) + 1.8;
                    scores_data[c* feature_size * feature_size +i* feature_size +j] = heatmap_data[i * feature_size * num_joints + j * num_joints + c] / dist_weight;
                }
            }
        }
        std::vector<int> kpts_ys, kpts_xs;
        for (int i = 0; i < num_joints; i++)
        {
            top_index = 0;
            top_score = 0;
            top_index = int(argmax(scores_data + feature_size * feature_size *i, scores_data + feature_size * feature_size *(i+1)));
            top_score = *std::max_element(scores_data + feature_size * feature_size * i, scores_data + feature_size * feature_size * (i + 1));

            int top_y = (top_index / feature_size);
            int top_x = top_index - top_y * feature_size;
            kpts_ys.push_back(top_y);
            kpts_xs.push_back(top_x);
        }

        points.clear();
        for (int i = 0; i < num_joints; i++)
        {
            float kpt_offset_x = offset_data[kpts_ys[i] * feature_size * num_joints*2 + kpts_xs[i] * num_joints * 2 + i * 2];
            float kpt_offset_y = offset_data[kpts_ys[i] * feature_size * num_joints * 2 + kpts_xs[i] * num_joints * 2 + i * 2+1];

            float x = (kpts_xs[i] + kpt_offset_y) * kpt_scale * target_size;
            float y = (kpts_ys[i] + kpt_offset_x) * kpt_scale * target_size;

            keypoint kpt;
            kpt.x = (x - (wpad / 2)) / scale;
            kpt.y = (y - (hpad / 2)) / scale;
            kpt.score = heatmap_data[kpts_ys[i] * feature_size * num_joints + kpts_xs[i] * num_joints + i];
            points.push_back(kpt);

        }

        return {};
    }

    int get_min_valid_keypoints() const noexcept
    {
        return min_valid_keypoints_;
    }
    float get_min_confidence() const noexcept
    {
        return min_confidence_;
    }

    void set_min_confidence(float val)
    {
        min_confidence_ = val;
    }
    void set_min_valid_keypoints(int val)
    {
        min_valid_keypoints_ = val;
    }

    std::string const param_;
    std::string const bin_;
    int load_model_success_ = -1;
    int load_param_success_ = -1;
    std::string input_name_;
    std::string output_name_;
    ncnn::Net net_;

    int target_size_;
    float min_confidence_;
    size_t min_valid_keypoints_;
    int feature_size_;
    float kpt_scale_;
    std::vector<float> const mean_vals_ = {127.5f, 127.5f,  127.5f};
    std::vector<float> norm_vals_ = {1/ 127.5f, 1 / 127.5f, 1 / 127.5f};

    std::vector<std::vector<float>> dist_y_;
    std::vector<std::vector<float>> dist_x_;
};

movenet_single_pose_estimate::movenet_single_pose_estimate(std::string const &param,
                                                           std::string const &bin,
                                                           int min_valid_keypoints,
                                                           float min_confidence,
                                                           int input_size) :
    pimpl_(std::make_unique<pimpl>(param, bin, input_size, min_valid_keypoints, min_confidence))
{

}

movenet_single_pose_estimate::~movenet_single_pose_estimate()
{

}

pose_estimation_results movenet_single_pose_estimate::predict(const cv::Mat &bgr)
{
    return pimpl_->detect(bgr);
}

float movenet_single_pose_estimate::get_min_confidence() const noexcept
{
    return pimpl_->get_min_confidence();
}

int movenet_single_pose_estimate::get_min_valid_keypoints() const noexcept
{
    return pimpl_->get_min_valid_keypoints();
}

void movenet_single_pose_estimate::set_min_confidence(float val)
{
    pimpl_->set_min_confidence(val);
}

void movenet_single_pose_estimate::set_min_valid_keypoints(int val)
{
    pimpl_->set_min_valid_keypoints(val);
}



}

