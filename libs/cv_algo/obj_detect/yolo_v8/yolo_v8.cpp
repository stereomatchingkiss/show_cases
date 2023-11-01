// Tencent is pleased to support the open source community by making ncnn available.
//
// Copyright (C) 2021 THL A29 Limited, a Tencent company. All rights reserved.
//
// Licensed under the BSD 3-Clause License (the "License"); you may not use this file except
// in compliance with the License. You may obtain a copy of the License at
//
// https://opensource.org/licenses/BSD-3-Clause
//
// Unless required by applicable law or agreed to in writing, software distributed
// under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
// CONDITIONS OF ANY KIND, either express or implied. See the License for the
// specific language governing permissions and limitations under the License.

//modified 1-14-2023 Q-engineering

#include "yolo_v8.hpp"

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

namespace flt::det{

namespace{

struct GridAndStride
{
    int grid0;
    int grid1;
    int stride;
};

float fast_exp(float x)
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

void qsort_descent_inplace(std::vector<box_info>& faceobjects, int left, int right)
{
    int i = left;
    int j = right;
    float p = faceobjects[(left + right) / 2].score_;

    while (i <= j)
    {
        while (faceobjects[i].score_ > p)
            ++i;

        while (faceobjects[j].score_ < p)
            --j;

        if (i <= j)
        {
            // swap
            std::swap(faceobjects[i], faceobjects[j]);

            ++i;
            --j;
        }
    }

    //     #pragma omp parallel sections
    {
        //         #pragma omp section
        {
            if (left < j) qsort_descent_inplace(faceobjects, left, j);
        }
        //         #pragma omp section
        {
            if (i < right) qsort_descent_inplace(faceobjects, i, right);
        }
    }
}

void qsort_descent_inplace(std::vector<box_info>& faceobjects)
{
    if (faceobjects.empty())
        return;

    qsort_descent_inplace(faceobjects, 0, static_cast<int>(faceobjects.size() - 1));
}

void nms_sorted_bboxes(const std::vector<box_info>& faceobjects,
                       std::vector<int>& picked,
                       float nms_threshold)
{    
    std::vector<float> areas(faceobjects.size());
    for(size_t i = 0; i < faceobjects.size(); ++i)
    {
        areas[i] = faceobjects[i].rect_.width * faceobjects[i].rect_.height;
    }

    picked.clear();
    for(int i = 0; i < faceobjects.size(); ++i)
    {
        auto const &a = faceobjects[i];
        bool keep = true;
        for(size_t j = 0; j < picked.size(); ++j){
            auto const &b = faceobjects[picked[j]];

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

void generate_grids_and_stride(int target_w,
                               int target_h,
                               std::vector<int>& strides,
                               std::vector<GridAndStride>& grid_strides)
{
    for(size_t i = 0; i < strides.size(); ++i){
        int const stride = strides[i];
        int const num_grid_w = target_w / stride;
        int const num_grid_h = target_h / stride;
        for(int g1 = 0; g1 < num_grid_h; ++g1){
            for(int g0 = 0; g0 < num_grid_w; ++g0){
                GridAndStride gs;
                gs.grid0 = g0;
                gs.grid1 = g1;
                gs.stride = stride;
                grid_strides.push_back(gs);
            }
        }
    }
}

void generate_proposals(std::vector<GridAndStride> grid_strides,
                        const ncnn::Mat& pred,
                        float prob_threshold,
                        int num_class,
                        std::vector<box_info>& objects)
{
    const int num_points = static_cast<int>(grid_strides.size());
    constexpr int reg_max_1 = 16;

    for(int i = 0; i < num_points; ++i)
    {
        const float* scores = pred.row(i) + 4 * reg_max_1;

        // find label with max score
        int label = -1;
        float score = -FLT_MAX;
        for(int k = 0; k < num_class; ++k)
        {
            float const confidence = scores[k];
            if(confidence > score)
            {
                label = k;
                score = confidence;
            }
        }
        float box_prob = sigmoid(score);
        if (box_prob >= prob_threshold)
        {
            ncnn::Mat bbox_pred(reg_max_1, 4, (void*)pred.row(i));
            {
                std::unique_ptr<ncnn::Layer> softmax(ncnn::create_layer("Softmax"));

                ncnn::ParamDict pd;
                pd.set(0, 1); // axis
                pd.set(1, 1);
                softmax->load_param(pd);

                ncnn::Option opt;
                opt.num_threads = 1;
                opt.use_packing_layout = false;

                softmax->create_pipeline(opt);

                softmax->forward_inplace(bbox_pred, opt);

                softmax->destroy_pipeline(opt);
                //delete softmax;
            }

            float pred_ltrb[4];
            for (int k = 0; k < 4; k++)
            {
                float dis = 0.f;
                const float* dis_after_sm = bbox_pred.row(k);
                for (int l = 0; l < reg_max_1; l++)
                {
                    dis += l * dis_after_sm[l];
                }

                pred_ltrb[k] = dis * grid_strides[i].stride;
            }

            float const pb_cx = (grid_strides[i].grid0 + 0.5f) * grid_strides[i].stride;
            float const pb_cy = (grid_strides[i].grid1 + 0.5f) * grid_strides[i].stride;
            objects.emplace_back(pb_cx - pred_ltrb[0],
                                 pb_cy - pred_ltrb[1],
                                 pb_cx + pred_ltrb[2],
                                 pb_cy + pred_ltrb[3],
                                 box_prob,
                                 label);
        }
    }
}

}

yolo_v8::yolo_v8(const char *param, const char *bin, int num_class, bool use_gpu, int input_size, int max_thread) :
    mean_vals_{103.53f, 116.28f, 123.675f},
    norm_vals_{1.0f / 255.0f, 1.0f / 255.0f, 1.0f / 255.0f},
    num_class_{num_class}
{
    net_.opt = ncnn::Option();
    net_.opt.num_threads = max_thread;
    net_.load_param(param);
    net_.load_model(bin);

    target_size_ = input_size;
}

yolo_v8::~yolo_v8()
{

}

std::vector<box_info> yolo_v8::predict(const cv::Mat &rgb, float score_threshold, float nms_threshold, int, bool)
{
    int width = rgb.cols;
    int height = rgb.rows;

    // pad to multiple of 32
    int w = width;
    int h = height;
    float scale = 1.f;
    if (w > h)
    {
        scale = static_cast<float>(target_size_) / w;
        w = target_size_;
        h = static_cast<int>(h * scale);
    }
    else
    {
        scale = static_cast<float>(target_size_) / h;
        h = target_size_;
        w = static_cast<int>(w * scale);
    }

    ncnn::Mat in = ncnn::Mat::from_pixels_resize(rgb.data, ncnn::Mat::PIXEL_RGB, width, height, w, h);

    // pad to target_size rectangle
    int const wpad = (w + 31) / 32 * 32 - w;
    int const hpad = (h + 31) / 32 * 32 - h;
    ncnn::Mat in_pad;
    ncnn::copy_make_border(in, in_pad, hpad / 2, hpad - hpad / 2, wpad / 2, wpad - wpad / 2, ncnn::BORDER_CONSTANT, 0.f);

    in_pad.substract_mean_normalize(0, norm_vals_);

    ncnn::Extractor ex = net_.create_extractor();

    ex.input("images", in_pad);

    ncnn::Mat out;
    ex.extract("output", out);

    std::vector<int> strides = {8, 16, 32}; // might have stride=64
    std::vector<GridAndStride> grid_strides;
    generate_grids_and_stride(in_pad.w, in_pad.h, strides, grid_strides);

    std::vector<box_info> proposals;
    generate_proposals(grid_strides, out, score_threshold, num_class_, proposals);

    // sort all proposals by score from highest to lowest
    qsort_descent_inplace(proposals);

    // apply nms with nms_threshold
    std::vector<int> picked;
    nms_sorted_bboxes(proposals, picked, nms_threshold);

    size_t const count = picked.size();
    std::vector<box_info> objects(count);
    for(size_t i = 0; i < count; i++){
        objects[i] = proposals[picked[i]];

        // adjust offset to original unpadded
        float x0 = (objects[i].rect_.x - (wpad / 2)) / scale;
        float y0 = (objects[i].rect_.y - (hpad / 2)) / scale;
        float x1 = (objects[i].rect_.x + objects[i].rect_.width - (wpad / 2)) / scale;
        float y1 = (objects[i].rect_.y + objects[i].rect_.height - (hpad / 2)) / scale;

        // clip
        x0 = std::max(std::min(x0, (width - 1.0f)), 0.f);
        y0 = std::max(std::min(y0, (height - 1.0f)), 0.f);
        x1 = std::max(std::min(x1, (width - 1.0f)), 0.f);
        y1 = std::max(std::min(y1, (height - 1.0f)), 0.f);

        objects[i].rect_.x = x0;
        objects[i].rect_.y = y0;
        objects[i].rect_.width = x1 - x0;
        objects[i].rect_.height = y1 - y0;
        objects[i].x2_ = x1;
        objects[i].y2_ = y1;
    }

    std::ranges::sort(objects, [](auto const &a, auto const &b){
        return a.rect_.area() > b.rect_.area();
    });

    return objects;
}

}
