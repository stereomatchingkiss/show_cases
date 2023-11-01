#include "draw_rect.hpp"

#include "../converter/qt_and_cv_rect_converter.hpp"

#include <opencv2/imgproc.hpp>

namespace flt::utils{

void draw_empty_rect(cv::Mat &inout, cv::Rect const &rect, int thickness)
{
    auto const tl = rect.tl();
    auto const tr = cv::Point(rect.x + rect.width, rect.y);
    auto const bl = cv::Point(rect.x, rect.y + rect.height);
    auto const br = rect.br();
    cv::line(inout, tl, tr, {0, 255, 125}, 4);
    cv::line(inout, tr, br, {0, 255, 125}, 4);
    cv::line(inout, br, bl, {0, 255, 125}, 4);
    cv::line(inout, bl, tl, {0, 255, 125}, 4);
}

void draw_empty_rect(cv::Mat &inout, QRectF const &rect, int thickness)
{
    draw_empty_rect(inout, convert_qrectf_to_cv_rect(rect, inout.cols, inout.rows));
}

}
