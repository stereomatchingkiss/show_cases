#ifndef PADDLE_OCR_TEXT_REC_HPP
#define PADDLE_OCR_TEXT_REC_HPP


#include "common.hpp"

#include <memory>
#include <string>

#include <opencv2/core.hpp>

namespace ocv::ocr{

class paddle_ocr_text_rec
{
public:
    /**
     * @param dst_height When use PP-OCRv3 it should be 48, PP-OCRv2 it should be 32
     */
    explicit paddle_ocr_text_rec(const char* param,
                                 const char* bin,
                                 const char *key_files,
                                 int dst_height = 48,
                                 bool use_gpu = false,
                                 int max_thread = 4);
    ~paddle_ocr_text_rec();

    bool get_load_model_success() const noexcept;
    bool get_load_param_success() const noexcept;
    int get_load_model_state() const noexcept;

    /**
     * @param src Expect input channles are RGB
     */
    void predict(cv::Mat const &src, std::vector<TextBox>& text_boxes);

private:
    struct impl;
    std::unique_ptr<impl> impl_;
};

}

#endif // PADDLE_OCR_TEXT_REC_HPP
