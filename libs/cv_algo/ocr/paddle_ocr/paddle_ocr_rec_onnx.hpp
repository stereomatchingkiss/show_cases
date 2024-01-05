#pragma once

#include <cv_algo/ocr/paddle_ocr/common.hpp>

#include <memory>
#include <string>
#include <vector>

namespace flt::cvt::ocr{

class paddle_ocr_rec_onnx
{
public:
    explicit paddle_ocr_rec_onnx(std::string const &model_weights,
                                 std::string const &key_files,
                                 int dst_height = 48,
                                 int max_width = 500);
    ~paddle_ocr_rec_onnx();

#ifdef WASM_BUILD
    bool predict_results_available() const;
    bool onnx_model_ready() const;
    void async_predict(cv::Mat const &mat, TextBox const &text_boxes);
    void predict(TextBox &text_boxes);
#else
    void predict(cv::Mat const &bgr, std::vector<TextBox>& text_boxes);
#endif

private:
    struct impl;
    std::unique_ptr<impl> impl_;
};

}
