#pragma once

#include <opencv2/core.hpp>
#include <QRect>
#include <QRectF>

namespace flt::cvt{

cv::Rect convert_qrectf_to_cv_rect(QRectF const &qrect, int scale_x, int scale_y);

QRect convert_cv_rect_to_qrect(cv::Rect const &rect, int scale_x = 1, int scale_y = 1);

}
