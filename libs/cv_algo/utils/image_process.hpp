#ifndef OCV_UTILS_IMAGE_PROCESS_2342KLJASDJSK_HPP
#define OCV_UTILS_IMAGE_PROCESS_2342KLJASDJSK_HPP

#include <opencv2/core.hpp>

namespace flt::cvt::utils{

void adjust_img_orientation(cv::Mat const &input, cv::Mat &output, int rotate_angle, bool horizontal_flip);

/**
 * Return cv::Rect within range
 */
cv::Rect clamp_rect(cv::Rect const &roi, cv::Size const &range);

void resize_aspect_ratio_to_height(cv::Mat const &src, cv::Mat &dst, int height);
void resize_aspect_ratio_to_width(cv::Mat const &src, cv::Mat &dst, int width);


void rotate_90n(cv::Mat const &src, cv::Mat &dst, int angle);

}

#endif
