#pragma once

#include "common.hpp"

#include <memory>
#include <string>

#include <opencv2/core.hpp>

namespace ocv::ocr{

class paddle_ocr_text_detector
{
public:
    explicit paddle_ocr_text_detector(const char* param,
                                      const char* bin,                                      
                                      bool use_gpu = false,
                                      int max_thread = 4);
    ~paddle_ocr_text_detector();

    bool get_load_model_success() const noexcept;
    bool get_load_param_success() const noexcept;
    int get_load_model_state() const noexcept;

    /**
     * @param src Expect input channles are RGB
     */
    std::vector<TextBox> predict(cv::Mat const &src, float box_score_thresh = 0.4f, float box_thresh = 0.3f, float un_clip_ratio = 2.0f);

private:
    struct impl;
    std::unique_ptr<impl> impl_;
};

}
