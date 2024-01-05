#pragma once

#include <cv_algo/ocr/paddle_ocr/common.hpp>

#include <memory>
#include <string>
#include <vector>

namespace flt::cvt::ocr{

class paddle_ocr_rec_opencv
{
public:
    explicit paddle_ocr_rec_opencv(std::string const &model_weights,
                                   std::string const &key_files,
                                   int dst_height = 48,
                                   int width = 320);
    ~paddle_ocr_rec_opencv();

    void predict(cv::Mat const &bgr, std::vector<TextBox>& text_boxes);

private:
    struct impl;
    std::unique_ptr<impl> impl_;
};

}
