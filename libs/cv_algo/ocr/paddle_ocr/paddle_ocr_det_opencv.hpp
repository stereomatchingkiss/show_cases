#pragma once

#include <cv_algo/ocr/paddle_ocr/common.hpp>

#include <string>

#include <opencv2/dnn.hpp>

namespace flt::cvt::ocr{

class paddle_ocr_det_opencv
{
public:
    explicit paddle_ocr_det_opencv(std::string const &model_weights,
                                   double binary_map_thresh = 0.3,
                                   float box_thresh = 0.3f,
                                   float unclip_ratio = 1.6f,
                                   bool use_dilate = false);
    ~paddle_ocr_det_opencv();

    std::vector<TextBox> predict(cv::Mat const &input);

private:
    cv::Mat get_input_mat(cv::Mat const &img, cv::Size const &resize_size) const;

    double binary_map_thresh_ = 0.3;
    float box_thresh_ = 0.3f;
    float unclip_ratio_ = 1.6f;
    bool use_dilate_ = false;

    cv::dnn::Net net_;
    std::vector<std::string> output_names_;
};

}
