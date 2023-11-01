#pragma once

#include <opencv2/core.hpp>

#include <QRectF>

namespace flt::cvt::utils{

void draw_empty_rect(cv::Mat &inout, QRectF const &rect, int thickness = 4);
void draw_empty_rect(cv::Mat &inout, cv::Rect const &rect, int thickness = 4);

}
