#include "qt_and_cv_rect_converter.hpp"

namespace flt::cvt{

cv::Rect convert_qrectf_to_cv_rect(const QRectF &qrect, int scale_x, int scale_y)
{
    cv::Rect rband;
    rband.x = qrect.x() * scale_x;
    rband.y = qrect.y() * scale_y;
    rband.width = qrect.width() * scale_x;
    rband.height = qrect.height() * scale_y;

    return rband;
}

QRect convert_cv_rect_to_qrect(const cv::Rect &rect, int scale_x, int scale_y)
{
    return QRect(rect.x, rect.y, rect.width, rect.height);
}

}
