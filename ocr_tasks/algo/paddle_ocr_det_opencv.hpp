#pragma once

#include <string>

#include <opencv2/dnn.hpp>

class paddle_ocr_det_opencv
{
public:
    explicit paddle_ocr_det_opencv(std::string const &model_weights);

    cv::Mat predict(cv::Mat const &input);

private:
    cv::dnn::Net net_;
};
