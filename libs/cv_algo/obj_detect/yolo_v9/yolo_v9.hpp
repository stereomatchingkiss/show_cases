#pragma once

#include "../common_obj_det_type.hpp"
#include "../obj_det_base.hpp"

#include <opencv2/core/core.hpp>
#include <net.h>

namespace flt::cvt::det{

class yolo_v9 : public obj_det_base
{
public:
    explicit yolo_v9(char const *param,
                     char const *bin,
                     int num_class,
                     bool use_gpu = false,
                     int input_size = 416,
                     int max_thread = 4);
    ~yolo_v9();

    std::vector<box_info> predict(cv::Mat const &rgb,
                                  float score_threshold = 0.4f,
                                  float nms_threshold = 0.5f,
                                  bool swap_channel = false) override;
private:
    ncnn::Extractor create_extractor() const;
    void generate_proposals(ncnn::Mat const &feat_blob, float prob_threshold, std::vector<box_info>& objects) const;
    ncnn::Mat preprocess(cv::Mat const &rgb, int w, int h, int dw, int dh, bool swap_channel) const;
    ncnn::Mat transpose(const ncnn::Mat& in) const;

    ncnn::Net net_;
    int target_size_;
    float const mean_vals_[3];
    float const norm_vals_[3];
    int num_class_;

    std::string input_name_;
    std::string output_name_;

    mutable std::mutex mutex_;
};

}
