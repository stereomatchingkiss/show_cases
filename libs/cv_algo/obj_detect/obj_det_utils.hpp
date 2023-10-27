#ifndef OBJ_DET_UTILS_HPP
#define OBJ_DET_UTILS_HPP

#include <opencv2/core.hpp>

namespace ocv{

struct box_info;
struct object_rect;

int resize_uniform(cv::Mat const &src, cv::Mat &dst, object_rect &effect_area, int dst_w, int dst_h);
void scale_bbox(int src_w, int src_h, std::vector<box_info> &bboxes, const object_rect &effect_roi);

}

#endif // OBJ_DET_UTILS_HPP
