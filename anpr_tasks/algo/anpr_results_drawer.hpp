#pragma once

#include "anpr_algo_predict_results.hpp"

#include <QPen>

class anpr_results_drawer
{
public:
    anpr_results_drawer();

    void draw(QImage &img, anpr_algo_predict_results const &result);
    void draw(QImage &img, std::vector<anpr_algo_predict_results> const &results);

private:
    QPen pen_plate_;
    QPen pen_text_;
    QPen pen_vechile_;
};
