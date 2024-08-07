#pragma once

#include <opencv2/core.hpp>

#include <QImage>

#include <any>

namespace flt::mm{

enum class stream_source_type;

}

class QRectF;

/**
 * @warning If source_type is rtsp, qimage is using the same memory of cv::Mat and vice versa
 */
std::pair<cv::Mat, QImage> convert_std_any_to_image(std::any frame,
                                                    flt::mm::stream_source_type source_type,
                                                    QRectF const &roi);

std::pair<cv::Mat, QImage> convert_std_any_to_image(std::any frame,
                                                    flt::mm::stream_source_type source_type);
