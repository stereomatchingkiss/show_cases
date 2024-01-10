#pragma once

#include "anpr_algo_predict_results.hpp"

#include <QImage>

struct anpr_worker_results
{
    QImage qimg_;
    std::vector<anpr_algo_predict_results> full_results_;
    std::vector<anpr_algo_predict_results> plate_results_;
};
