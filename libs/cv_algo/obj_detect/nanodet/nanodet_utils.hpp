#ifndef NANODET_UTILS_HPP
#define NANODET_UTILS_HPP

#include <opencv2/core.hpp>

namespace flt::cvt::det{

class nanodet;

std::tuple<int, cv::Mat> predict_image(nanodet &net, std::string const &img_path,
                                       std::vector<std::string> const &names,
                                       float conf = 0.4f,
                                       float nms = 0.3f);

std::pair<int, std::string> predict_bboxes(cv::Mat const &input,
                                           nanodet &net,
                                           int rotate_angle = 0,
                                           bool horizontal_flip = false,
                                           float conf_threshold = 0.3f,
                                           float nms_threshold = 0.4f);

}

#endif
