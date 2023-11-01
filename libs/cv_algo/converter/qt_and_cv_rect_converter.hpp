#pragma once

#include <opencv2/core.hpp>
#include <QRectF>

namespace flt::cvt{

cv::Rect convert_qrectf_to_cv_rect(QRectF const &qrect, int scale_x, int scale_y);

}
