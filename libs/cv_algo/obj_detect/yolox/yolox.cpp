#include "yolox.hpp"

#include "../common_obj_det_type.hpp"
#include "../ncnn_det_model_auxiliary.hpp"

#include "../layer/yolo_v5_focus.hpp"

namespace flt::cvt::det{

namespace{

static void generate_yolox_proposals(std::vector<grid_and_stride> grid_strides,
                                     const ncnn::Mat& feat_blob,
                                     float prob_threshold,
                                     std::vector<box_info>& objects)
{
    //    const int num_grid = feat_blob.h;
    const int num_class = feat_blob.w - 5;
    const size_t num_anchors = grid_strides.size();

    const float* feat_ptr = feat_blob.channel(0);
    for (int anchor_idx = 0; anchor_idx < num_anchors; ++anchor_idx){
        const int grid0 = grid_strides[anchor_idx].grid0;
        const int grid1 = grid_strides[anchor_idx].grid1;
        const int stride = grid_strides[anchor_idx].stride;

        // yolox/models/yolo_head.py decode logic
        //  outputs[..., :2] = (outputs[..., :2] + grids) * strides
        //  outputs[..., 2:4] = torch.exp(outputs[..., 2:4]) * strides
        float x_center = (feat_ptr[0] + grid0) * stride;
        float y_center = (feat_ptr[1] + grid1) * stride;
        float w = exp(feat_ptr[2]) * stride;
        float h = exp(feat_ptr[3]) * stride;
        float x0 = x_center - w * 0.5f;
        float y0 = y_center - h * 0.5f;

        float box_objectness = feat_ptr[4];
        for (int class_idx = 0; class_idx < num_class; class_idx++){
            float box_cls_score = feat_ptr[5 + class_idx];
            float box_prob = box_objectness * box_cls_score;
            if (box_prob > prob_threshold){
                objects.emplace_back(x0, y0, x0 + w, y0 + h, box_prob, class_idx);
            }
        } // class loop
        feat_ptr += feat_blob.w;
    } // point anchor loop
}

}

yolox::yolox(const char *param, const char *bin, bool use_gpu, int input_size, int max_thread)
{
    net_.opt = ncnn::Option();
    net_.opt.num_threads = max_thread;
    net_.opt.use_fp16_arithmetic = true;
    net_.register_custom_layer("YoloV5Focus", yolov5focus_layer_creater);

#if NCNN_VULKAN
    net_.opt.use_vulkan_compute = ncnn::get_gpu_count() > 0 && use_gpu;
#endif

    target_size_ = input_size;

    net_.load_param(param);
    net_.load_model(bin);
}

yolox::~yolox()
{

}

std::vector<box_info> yolox::predict(const cv::Mat &bgr, float score_threshold, float nms_threshold, int rotation_angle, bool hflip)
{
    int const img_w = bgr.cols;
    int const img_h = bgr.rows;

    // letterbox pad to multiple of 32
    int w = img_w;
    int h = img_h;
    float scale = 1.f;
    if (w > h)
    {
        scale = static_cast<float>(target_size_) / w;
        w = target_size_;
        h = h * scale;
    }
    else
    {
        scale = static_cast<float>(target_size_) / h;
        h = target_size_;
        w = w * scale;
    }

    ncnn::Mat in = ncnn::Mat::from_pixels_resize(bgr.data, ncnn::Mat::PIXEL_BGR, img_w, img_h, w, h);

    // pad to target_size rectangle
    int const wpad = target_size_ - w;
    int const hpad = target_size_ - h;
    ncnn::Mat in_pad;
    // different from yolov5, yolox only pad on bottom and right side,
    // which means users don't need to extra padding info to decode boxes coordinate.
    ncnn::copy_make_border(in, in_pad, 0, hpad, 0, wpad, ncnn::BORDER_CONSTANT, 114.f);

    ncnn::Extractor ex = net_.create_extractor();

    ex.input("images", in_pad);

    std::vector<box_info> proposals;

    {
        ncnn::Mat out;
        ex.extract("output", out);

        static const int stride_arr[] = {8, 16, 32}; // might have stride=64 in YOLOX
        std::vector<int> strides(stride_arr, stride_arr + sizeof(stride_arr) / sizeof(stride_arr[0]));
        std::vector<grid_and_stride> grid_strides;
        generate_grids_and_stride(target_size_, target_size_, strides, grid_strides);
        generate_yolox_proposals(grid_strides, out, score_threshold, proposals);
    }
    // sort all proposals by score from highest to lowest
    qsort_descent_inplace(proposals);

    // apply nms with nms_threshold
    std::vector<int> picked;
    nms_sorted_bboxes(proposals, picked, nms_threshold);

    int count = picked.size();

    std::vector<box_info> objects(count);
    for(int i = 0; i < count; ++i){
        objects[i] = proposals[picked[i]];

        // adjust offset to original unpadded
        float x0 = (objects[i].rect_.x) / scale;
        float y0 = (objects[i].rect_.y) / scale;
        float x1 = (objects[i].rect_.x + objects[i].rect_.width) / scale;
        float y1 = (objects[i].rect_.y + objects[i].rect_.height) / scale;

        // clip
        x0 = std::max(std::min(x0, static_cast<float>(img_w - 1)), 0.f);
        y0 = std::max(std::min(y0, static_cast<float>(img_h - 1)), 0.f);
        x1 = std::max(std::min(x1, static_cast<float>(img_w - 1)), 0.f);
        y1 = std::max(std::min(y1, static_cast<float>(img_h - 1)), 0.f);

        objects[i].rect_.x = x0;
        objects[i].rect_.y = y0;
        objects[i].rect_.width = x1 - x0;
        objects[i].rect_.height = y1 - y0;
        objects[i].x2_ = x1;
        objects[i].y2_ = y1;
    }

    return objects;
}

}
