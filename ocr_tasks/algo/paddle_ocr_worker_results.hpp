#pragma once

#include <cv_algo/ocr/paddle_ocr/common.hpp>

#include <QImage>

struct paddle_ocr_worker_results
{
    QImage mat_;
    cv::Mat cv_mat_;

    std::vector<flt::cvt::ocr::TextBox> text_boxes_;
};
