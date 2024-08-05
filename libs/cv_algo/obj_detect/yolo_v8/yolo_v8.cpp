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

#include "../ncnn_det_model_auxiliary.hpp"

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

namespace flt::cvt::det{

namespace{

void generate_proposals(std::vector<grid_and_stride> grid_strides,
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
    obj_det_base(),
    mean_vals_{103.53f, 116.28f, 123.675f},
    norm_vals_{1.0f / 255.0f, 1.0f / 255.0f, 1.0f / 255.0f},
    num_class_{num_class},
    target_size_{input_size}
{
    std::lock_guard<std::mutex> lock(mutex_);
    net_.opt = ncnn::Option();
    net_.opt.num_threads = max_thread;

#if NCNN_VULKAN
    net_.opt.use_vulkan_compute = ncnn::get_gpu_count() > 0 && use_gpu;
#endif

    net_.load_param(param);
    net_.load_model(bin);

    input_name_ = net_.input_names()[0];
    output_name_ = net_.output_names()[net_.output_names().size() - 1];    
}

yolo_v8::~yolo_v8()
{

}

std::vector<box_info> yolo_v8::predict(cv::Mat const &rgb, float score_threshold, float nms_threshold, int, bool)
{
    //pad to multiple of 32
    auto const [w, h, scale] = pad_to_multiple_of_det_model(rgb.cols, rgb.rows, target_size_);

    ncnn::Mat in = ncnn::Mat::from_pixels_resize(rgb.data, ncnn::Mat::PIXEL_RGB, rgb.cols, rgb.rows, w, h);

    // pad to target_size rectangle
    int const wpad = (w + 31) / 32 * 32 - w;
    int const hpad = (h + 31) / 32 * 32 - h;
    ncnn::Mat in_pad;
    ncnn::copy_make_border(in, in_pad, hpad / 2, hpad - hpad / 2, wpad / 2, wpad - wpad / 2, ncnn::BORDER_CONSTANT, 0.f);

    in_pad.substract_mean_normalize(0, norm_vals_);

    ncnn::Extractor ex = create_extractor();

    ex.input(input_name_.c_str(), in_pad);

    ncnn::Mat out;
    ex.extract(output_name_.c_str(), out);

    std::vector<int> strides = {8, 16, 32}; // might have stride=64
    std::vector<grid_and_stride> grid_strides;
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
        x0 = std::max(std::min(x0, (rgb.cols - 1.0f)), 0.f);
        y0 = std::max(std::min(y0, (rgb.rows - 1.0f)), 0.f);
        x1 = std::max(std::min(x1, (rgb.cols - 1.0f)), 0.f);
        y1 = std::max(std::min(y1, (rgb.rows - 1.0f)), 0.f);

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

ncnn::Extractor yolo_v8::create_extractor() const
{
    std::lock_guard<std::mutex> lock(mutex_);
    return net_.create_extractor();
}

}
