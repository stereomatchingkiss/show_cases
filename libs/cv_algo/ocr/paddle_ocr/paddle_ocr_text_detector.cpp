#include "paddle_ocr_text_detector.hpp"

#include "common.hpp"

#include <net.h>

#include <format>

namespace flt::cvt::ocr{

namespace{

std::vector<TextBox> findRsBoxes(cv::Mat const &fMapMat,
                                 cv::Mat const &norfMapMat,
                                 float boxScoreThresh,
                                 float unClipRatio)
{
    float minArea = 3;
    std::vector<TextBox> rsBoxes;
    rsBoxes.clear();
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(norfMapMat, contours, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE);
    std::cout<<"contours size = "<<contours.size()<<std::endl;
    for (int i = 0; i < contours.size(); ++i)
    {
        float minSideLen, perimeter;
        std::vector<cv::Point> minBox = getMinBoxes(contours[i], minSideLen, perimeter);
        std::cout<<std::format("{}: minSideLen = {}", i, minSideLen);
        if (minSideLen < minArea)
            continue;
        float score = boxScoreFast(fMapMat, contours[i]);
        std::cout<<std::format("{}: score = {}", i, score);
        if (score < boxScoreThresh)
            continue;
        //---use clipper start---
        std::vector<cv::Point> clipBox = unClip(minBox, perimeter, unClipRatio);
        std::vector<cv::Point> clipMinBox = getMinBoxes(clipBox, minSideLen, perimeter);
        //---use clipper end---

        std::cout<<std::format("clipBox size = {}, clipMinBox size = {}", clipBox.size(), clipMinBox.size());

        if (minSideLen < minArea + 2)
            continue;

        for (int j = 0; j < clipMinBox.size(); ++j)
        {
            clipMinBox[j].x = static_cast<int>(clipMinBox[j].x / 1.0);
            clipMinBox[j].x = (std::min)((std::max)(clipMinBox[j].x, 0), norfMapMat.cols);

            clipMinBox[j].y = static_cast<int>(clipMinBox[j].y / 1.0);
            clipMinBox[j].y = (std::min)((std::max)(clipMinBox[j].y, 0), norfMapMat.rows);
        }

        rsBoxes.emplace_back(TextBox{ clipMinBox, score });
    }
    reverse(rsBoxes.begin(), rsBoxes.end());

    return rsBoxes;
}

}

struct paddle_ocr_text_detector::impl
{
    impl(const char *param, const char *bin, bool use_gpu, int max_thread)
    {
#if NCNN_VULKAN
        has_gpu_ = ncnn::get_gpu_count() > 0 && use_gpu;
#endif
        net_.opt.use_vulkan_compute = has_gpu_;
        net_.opt.use_fp16_arithmetic = true;
        net_.opt.num_threads = max_thread_;
        load_param_success_ = net_.load_param(param);
        load_model_success_ = net_.load_model(bin);

        input_name_ = net_.input_names()[0];
        output_name_ = net_.output_names()[net_.output_names().size() - 1];
    }

    std::vector<TextBox> predict(const cv::Mat &src, float box_score_thresh, float box_thresh, float un_clip_ratio);

    bool has_gpu_ = false;
    int load_model_success_ = -1;
    int load_param_success_ = -1;
    int max_thread_ = 4;
    std::string input_name_;
    std::string output_name_;
    ncnn::Net net_;
};

paddle_ocr_text_detector::paddle_ocr_text_detector(const char *param, const char *bin, bool use_gpu, int max_thread) :
    impl_{std::make_unique<impl>(param, bin, use_gpu, max_thread)}
{

}

paddle_ocr_text_detector::~paddle_ocr_text_detector()
{

}

bool paddle_ocr_text_detector::get_load_model_success() const noexcept
{
    return impl_->load_model_success_;
}

bool paddle_ocr_text_detector::get_load_param_success() const noexcept
{
    return impl_->load_param_success_;
}

int paddle_ocr_text_detector::get_load_model_state() const noexcept
{
    if(get_load_model_success() == 0 && get_load_param_success() == 0){
        return 0;
    }
    if(get_load_model_success() == true && get_load_param_success() == false){
        return 1;
    }
    if(get_load_model_success() == false && get_load_param_success() == true){
        return 2;
    }
    if(get_load_model_success() == false && get_load_param_success() == false){
        return 3;
    }

    return -1;
}

std::vector<TextBox> paddle_ocr_text_detector::predict(cv::Mat const &src, float box_score_thresh, float box_thresh, float un_clip_ratio)
{
    return impl_->predict(src, box_score_thresh, box_thresh, un_clip_ratio);
}

std::vector<TextBox> paddle_ocr_text_detector::impl::predict(const cv::Mat &src, float box_score_thresh, float box_thresh, float un_clip_ratio)
{
    int const width = src.cols;
    int const height = src.rows;
    int constexpr target_size = 640;
    // pad to multiple of 32
    int w = width;
    int h = height;
    float scale = 1.f;
    if (w > h)
    {
        scale = static_cast<float>(target_size) / w;
        w = target_size;
        h = static_cast<int>(h * scale);
    }
    else
    {
        scale = static_cast<float>(target_size) / h;
        h = target_size;
        w = static_cast<int>(w * scale);
    }
    std::cout<<std::format("h = {}, w = {}, scale = {}", h, w, scale)<<std::endl;

    ncnn::Mat input = ncnn::Mat::from_pixels_resize(src.data, ncnn::Mat::PIXEL_RGB, width, height, w, h);

    // pad to target_size rectangle
    int const wpad = (w + 31) / 32 * 32 - w;
    int const hpad = (h + 31) / 32 * 32 - h;

    std::cout<<std::format("wpad = {}, hpad = {}", wpad, hpad)<<std::endl;

    ncnn::Mat in_pad;
    ncnn::copy_make_border(input, in_pad, hpad / 2, hpad - hpad / 2, wpad / 2, wpad - wpad / 2, ncnn::BORDER_CONSTANT, 0.f);

    constexpr float meanValues[3] = { 0.485f * 255, 0.456f * 255, 0.406f * 255 };
    constexpr float normValues[3] = { 1.0f / 0.229f / 255.0f, 1.0f / 0.224f / 255.0f, 1.0f / 0.225f / 255.0f };

    in_pad.substract_mean_normalize(meanValues, normValues);
    ncnn::Extractor extractor = net_.create_extractor();

    extractor.input(input_name_.c_str(), in_pad);
    ncnn::Mat out;
    extractor.extract(output_name_.c_str(), out);

    std::cout<<std::format("input_name_ = {}, output_name_ = {}", input_name_, output_name_)<<std::endl;

    cv::Mat fMapMat(in_pad.h, in_pad.w, CV_32FC1, (float*)out.data);
    cv::Mat norfMapMat;
    norfMapMat = fMapMat > box_thresh;

    cv::dilate(norfMapMat, norfMapMat, cv::Mat(), cv::Point(-1, -1), 1);

    std::cout<<std::format("norfMapMat rows = {}, norfMapMat cols = {}", norfMapMat.rows, norfMapMat.cols)<<std::endl;
    for(size_t i = 0; i != 10; ++i){
        std::cout<<"norfMapMat:"<<i<<":"<<norfMapMat.at<cv::Vec3b>(i)<<std::endl;
    }
    for(size_t i = norfMapMat.rows; i != norfMapMat.rows + 10; ++i){
        std::cout<<"norfMapMat:"<<i<<":"<<norfMapMat.at<cv::Vec3b>(i)<<std::endl;
    }
    std::vector<TextBox> result = findRsBoxes(fMapMat, norfMapMat, box_score_thresh, 2.0f);
    for(size_t i = 0; i < result.size(); ++i){
        for(size_t j = 0; j < result[i].boxPoint.size(); ++j){
            float x = (result[i].boxPoint[j].x-(wpad/2))/scale;
            float y = (result[i].boxPoint[j].y-(hpad/2))/scale;
            x = std::max(std::min(x,(float)(width-1)),0.f);
            y = std::max(std::min(y,(float)(height-1)),0.f);
            result[i].boxPoint[j].x = static_cast<int>(x);
            result[i].boxPoint[j].y = static_cast<int>(y);
        }
    }

    return result;
}

}
