#include "yolo_v9.hpp"

#include "../ncnn_det_model_auxiliary.hpp"

namespace flt::cvt::det{

yolo_v9::yolo_v9(const char *param, const char *bin, int num_class, bool use_gpu, int input_size, int max_thread) :
    obj_det_base(),
    mean_vals_{103.53f, 116.28f, 123.675f},
    norm_vals_{1.0f / 255.0f, 1.0f / 255.0f, 1.0f / 255.0f},
    num_class_{num_class}
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

    target_size_ = input_size;
}

yolo_v9::~yolo_v9()
{

}

std::vector<box_info> yolo_v9::predict(const cv::Mat &rgb,
                                       float score_threshold,
                                       float nms_threshold,
                                       bool swap_channel)
{    
    // pad to multiple of 32
    auto const [w, h, scale] = pad_to_multiple_of_det_model(rgb.cols, rgb.rows, target_size_);

    int const dw = (target_size_ - w) / 2;
    int const dh = (target_size_ - h) / 2;

    ncnn::Extractor ex = create_extractor();
    ex.input(input_name_.c_str(), preprocess(rgb, w, h, dw, dh, swap_channel));

    ncnn::Mat out;
    ex.extract(output_name_.c_str(), out);

    std::vector<box_info> proposals;
    //transpose turn CHW->CWH  (1,84,8400)->(1,8400,84)
    generate_proposals(transpose(out), score_threshold, proposals);

    // sort all proposals by score from highest to lowest
    qsort_descent_inplace(proposals);

    // apply nms with nms_threshold
    std::vector<int> picked;
    nms_sorted_bboxes(proposals, picked, nms_threshold);

    std::vector<box_info> objects;    
    for(size_t i = 0; i < picked.size(); ++i){
        auto const &obj = proposals[picked[i]];
        // adjust offset to original unpadded
        float x0 = (obj.rect_.x - dw) / scale;
        float y0 = (obj.rect_.y - dh) / scale;
        float x1 = (obj.rect_.x + obj.rect_.width - dw) / scale;
        float y1 = (obj.rect_.y + obj.rect_.height - dh) / scale;

        //clip
        x0 = std::max(std::min(x0, (float)(rgb.cols - 1)), 0.f);
        y0 = std::max(std::min(y0, (float)(rgb.rows - 1)), 0.f);
        x1 = std::max(std::min(x1, (float)(rgb.cols - 1)), 0.f);
        y1 = std::max(std::min(y1, (float)(rgb.rows - 1)), 0.f);

        objects.emplace_back(x0, y0, x1, y1, obj.score_, obj.label_);
    }

    return objects;
}

ncnn::Extractor yolo_v9::create_extractor() const
{
    std::lock_guard<std::mutex> lock(mutex_);
    return net_.create_extractor();
}

void yolo_v9::generate_proposals(ncnn::Mat const &feat_blob, float prob_threshold, std::vector<box_info> &objects) const
{
    float const *data = static_cast<float*>(feat_blob.data);
    for(int i = 0; i < feat_blob.h; ++i){
        for(int j = 4; j < feat_blob.w; ++j){
            if(data[i * feat_blob.w + j] >= prob_threshold){
                int const class_index = j - 4;
                float const confidence = data[i * feat_blob.w + j];
                float const x0 = data[i * feat_blob.w] - data[i * feat_blob.w + 2] * 0.5f;
                float const y0 = data[i * feat_blob.w + 1] - data[i*feat_blob.w + 3] * 0.5f;
                float const x1 = data[i * feat_blob.w] + data[i * feat_blob.w + 2] * 0.5f;
                float const y1 = data[i * feat_blob.w + 1] + data[i * feat_blob.w + 3] * 0.5f;

                objects.emplace_back(x0, y0, x1, y1, confidence, class_index);
            }
        }
    }
}

ncnn::Mat yolo_v9::preprocess(cv::Mat const &rgb, int w, int h, int dw, int dh, bool swap_channel) const
{
    ncnn::Mat in;
    if(swap_channel){
        in = ncnn::Mat::from_pixels_resize(rgb.data, ncnn::Mat::PIXEL_BGR2RGB, rgb.cols, rgb.rows, w, h);
    }else{
        in = ncnn::Mat::from_pixels_resize(rgb.data, ncnn::Mat::PIXEL_RGB, rgb.cols, rgb.rows, w, h);
    }
    // pad to target_size rectangle left, right = int(round(dw - 0.1)), int(round(dw + 0.1))
    int const top = static_cast<int>(std::round(dh - 0.1));
    int const bottom = static_cast<int>(std::round(dh + 0.1));
    int const left = static_cast<int>(std::round(dw - 0.1));
    int const right = static_cast<int>(std::round(dw + 0.1));

    ncnn::Mat in_pad;
    ncnn::copy_make_border(in, in_pad, top, bottom, left, right, ncnn::BORDER_CONSTANT, 114.f);

    in_pad.substract_mean_normalize(0, norm_vals_);

    return in_pad;
}

ncnn::Mat yolo_v9::transpose(ncnn::Mat const &in) const
{
    ncnn::Mat out(in.h, in.w, in.c);
    for(int q = 0; q < in.c; ++q) {
        float const *ptr = in.channel(q);
        float *outptr = out.channel(q);
        for(size_t y = 0; y < in.h; ++y){
            for(size_t x = 0; x<  in.w; ++x){
                outptr[x * in.h + y] = ptr[y * in.w + x];
            }
        }
    }

    return out;
}



}
