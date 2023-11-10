#include "ncnn_det_model_auxiliary.hpp"

namespace flt::cvt::det{

void qsort_descent_inplace(std::vector<box_info> &face_objects, int left, int right)
{
    int i = left;
    int j = right;
    float p = face_objects[(left + right) / 2].score_;

    while (i <= j)
    {
        while (face_objects[i].score_ > p)
            ++i;

        while (face_objects[j].score_ < p)
            --j;

        if (i <= j)
        {
            // swap
            std::swap(face_objects[i], face_objects[j]);

            ++i;
            --j;
        }
    }

    //     #pragma omp parallel sections
    {
        //         #pragma omp section
        {
            if (left < j) qsort_descent_inplace(face_objects, left, j);
        }
        //         #pragma omp section
        {
            if (i < right) qsort_descent_inplace(face_objects, i, right);
        }
    }
}

void qsort_descent_inplace(std::vector<box_info> &face_bjects)
{
    if (face_bjects.empty())
        return;

    qsort_descent_inplace(face_bjects, 0, static_cast<int>(face_bjects.size() - 1));
}

void nms_sorted_bboxes(const std::vector<box_info> &face_objects, std::vector<int> &picked, float nms_threshold)
{
    std::vector<float> areas(face_objects.size());
    for(size_t i = 0; i < face_objects.size(); ++i)
    {
        areas[i] = face_objects[i].rect_.width * face_objects[i].rect_.height;
    }

    picked.clear();
    for(int i = 0; i < face_objects.size(); ++i)
    {
        auto const &a = face_objects[i];
        bool keep = true;
        for(size_t j = 0; j < picked.size(); ++j){
            auto const &b = face_objects[picked[j]];

            // intersection over union
            float const inter_area = intersection_area(a, b);
            float const union_area = areas[i] + areas[picked[j]] - inter_area;
            // float IoU = inter_area / union_area
            if (inter_area / union_area > nms_threshold)
                keep = false;
        }

        if(keep)
            picked.push_back(i);
    }
}

void generate_grids_and_stride(int target_w, int target_h, std::vector<int> &strides, std::vector<grid_and_stride> &grid_strides)
{
    for(size_t i = 0; i < strides.size(); ++i){
        int const stride = strides[i];
        int const num_grid_w = target_w / stride;
        int const num_grid_h = target_h / stride;
        for(int g1 = 0; g1 < num_grid_h; ++g1){
            for(int g0 = 0; g0 < num_grid_w; ++g0){
                grid_and_stride gs;
                gs.grid0 = g0;
                gs.grid1 = g1;
                gs.stride = stride;
                grid_strides.push_back(gs);
            }
        }
    }
}



}
