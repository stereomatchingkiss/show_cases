#pragma once

#include "anpr_algo_predict_results.hpp"

#include <QImage>

#include <memory>

struct config_object_detect_model_select;

class anpr_algo
{
public:
    explicit anpr_algo(config_object_detect_model_select const &config_model_select);
    ~anpr_algo();

    std::vector<anpr_algo_predict_results> predict(QImage &rgb);
    std::vector<anpr_algo_predict_results> predict(cv::Mat const &bgr);

private:
    struct impl;
    std::unique_ptr<impl> impl_;
};
