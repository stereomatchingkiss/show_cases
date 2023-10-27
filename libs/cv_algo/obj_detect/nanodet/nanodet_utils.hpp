#ifndef NANODET_UTILS_HPP
#define NANODET_UTILS_HPP

#include "../common_obj_det_type.hpp"

namespace ocv::det{

class nanodet;

void draw_bboxes(cv::Mat& image,
                 const std::vector<box_info>& bboxes,
                 std::vector<std::string> const &class_names);

void draw_bboxes_custom(cv::Mat& image,
                        box_info const &bboxes,
                        std::string const &message);

void draw_bboxes_custom(cv::Mat& image,
                        std::vector<box_info> const &bboxes,
                        std::vector<std::string> const &message);

void draw_output_strings_results(cv::Mat &image, std::vector<std::string> const &outputs, std::vector<std::string> const &class_names);

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
