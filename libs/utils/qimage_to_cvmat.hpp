#pragma once

#include <QImage>

#include <opencv2/core.hpp>

namespace flt{

/**
 * @param gray_to_color If true, it will convert the gray image to RGB color, else remain as gray image
 * @return First : image after converted.
 * Second : True if non copy, false if copied
 */
std::pair<cv::Mat, bool> qimg_convert_to_cvmat_non_copy(QImage &qimg, bool gray_to_color = true);

}
