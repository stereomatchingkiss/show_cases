#ifndef OBJ_DET_UTILS_HPP
#define OBJ_DET_UTILS_HPP

#include <opencv2/core.hpp>

namespace flt::det{

struct box_info;
struct object_rect;

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


int resize_uniform(cv::Mat const &src, cv::Mat &dst, object_rect &effect_area, int dst_w, int dst_h);
void scale_bbox(int src_w, int src_h, std::vector<box_info> &bboxes, const object_rect &effect_roi);

}

#endif // OBJ_DET_UTILS_HPP
