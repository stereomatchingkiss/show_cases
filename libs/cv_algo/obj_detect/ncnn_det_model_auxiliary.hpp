#pragma once

#include "common_obj_det_type.hpp"

#include <vector>

#include <opencv2/core.hpp>

namespace flt::cvt::det{

struct grid_and_stride
{
    int grid0;
    int grid1;
    int stride;
};

inline float fast_exp(float x)
{
    union {
        uint32_t i;
        float f;
    } v{};
    v.i = (1 << 23) * (1.4426950409 * x + 126.93490512f);
    return v.f;
}

inline float sigmoid(float x)
{
    return 1.0f / (1.0f + fast_exp(-x));
}

inline float intersection_area(const box_info& a, const box_info& b)
{
    return (a.rect_ & b.rect_).area();
}

void qsort_descent_inplace(std::vector<box_info>& faceobjects, int left, int right);

void qsort_descent_inplace(std::vector<box_info>& faceobjects);

void nms_sorted_bboxes(const std::vector<box_info>& faceobjects,
                       std::vector<int>& picked,
                       float nms_threshold);

void generate_grids_and_stride(int target_w,
                               int target_h,
                               std::vector<int>& strides,
                               std::vector<grid_and_stride>& grid_strides);

}
