#include "image_process.hpp"

#include "../obj_detect/common_obj_det_type.hpp"

#include <opencv2/imgproc.hpp>

#include <algorithm>

namespace ocv::utils{

void adjust_img_orientation(const cv::Mat &input, cv::Mat &output, int rotate_angle, bool horizontal_flip)
{
    if(rotate_angle != 0){
        ocv::utils::rotate_90n(input, output, rotate_angle);
    }
    if(horizontal_flip){
        if(output.empty()){
            cv::flip(input, output, 1);
        }else{
            cv::flip(output, output, 1);
        }
    }
    if(output.empty()){
        output = input;
    }
}

void rotate_90n(const cv::Mat &src, cv::Mat &dst, int angle)
{
    CV_Assert(angle % 90 == 0 && angle <= 360 && angle >= -360);
    if(angle == 270 || angle == -90){
        // Rotate clockwise 270 degrees
        cv::transpose(src, dst);
        cv::flip(dst, dst, 0);
    }else if(angle == 180 || angle == -180){
        // Rotate clockwise 180 degrees
        cv::flip(src, dst, -1);
    }else if(angle == 90 || angle == -270){
        // Rotate clockwise 90 degrees
        cv::transpose(src, dst);
        cv::flip(dst, dst, 1);
    }else if(angle == 360 || angle == 0 || angle == -360){
        if(src.data != dst.data){
            src.copyTo(dst);
        }
    }
}

cv::Rect clamp_rect(const cv::Rect &roi, const cv::Size &range)
{    
    auto rect = roi;
    rect.x = std::clamp(roi.x, 0,  range.width - 2);
    rect.y = std::clamp(roi.y, 0, range.height - 2);
    rect.width = std::clamp(roi.width, 1, range.width - rect.x - 1);
    rect.height = std::clamp(roi.height, 1, range.height - rect.y - 1);

    return rect;
}

int resize_uniform(const cv::Mat &src, cv::Mat &dst, object_rect &effect_area, int dst_w, int dst_h)
{
    int const w = src.cols;
    int const h = src.rows;
    //std::cout << "src: (" << h << ", " << w << ")" << std::endl;
    dst = cv::Mat(cv::Size(dst_w, dst_h), CV_8UC3, cv::Scalar(0));

    float ratio_src = w * 1.0f / h;
    float ratio_dst = dst_w * 1.0f / dst_h;

    int tmp_w = 0;
    int tmp_h = 0;
    if (ratio_src > ratio_dst) {
        tmp_w = dst_w;
        tmp_h = static_cast<int>(std::floor((dst_w * 1.0f / w) * h));
    }
    else if (ratio_src < ratio_dst) {
        tmp_h = dst_h;
        tmp_w = static_cast<int>(std::floor((dst_h * 1.0f / h) * w));
    }
    else {
        cv::resize(src, dst, cv::Size(dst_w, dst_h));
        effect_area.x_ = 0;
        effect_area.y_ = 0;
        effect_area.width_ = dst_w;
        effect_area.height_ = dst_h;
        return 0;
    }

    //std::cout << "tmp: (" << tmp_h << ", " << tmp_w << ")" << std::endl;
    cv::Mat tmp;
    cv::resize(src, tmp, cv::Size(tmp_w, tmp_h));

    if (tmp_w != dst_w) {
        int index_w = static_cast<int>(std::floor((dst_w - tmp_w) / 2.0f));
        //std::cout << "index_w: " << index_w << std::endl;
        for (int i = 0; i < dst_h; i++) {
            memcpy(dst.data + i * dst_w * 3 + index_w * 3, tmp.data + i * tmp_w * 3, tmp_w * 3);
        }
        effect_area.x_ = index_w;
        effect_area.y_ = 0;
        effect_area.width_ = tmp_w;
        effect_area.height_ = tmp_h;
    }
    else if (tmp_h != dst_h) {
        int index_h = static_cast<int>(std::floor((dst_h - tmp_h) / 2.0f));
        memcpy(dst.data + index_h * dst_w * 3, tmp.data, tmp_w * tmp_h * 3);
        effect_area.x_ = 0;
        effect_area.y_ = index_h;
        effect_area.width_ = tmp_w;
        effect_area.height_ = tmp_h;
    }
    else {
        printf("error\n");
    }

    return 0;
}

void scale_bbox(int src_w, int src_h, std::vector<box_info> &bboxes, const object_rect &effect_roi)
{
    float const width_ratio = static_cast<float>(src_w) / static_cast<float>(effect_roi.width_);
    float const height_ratio = static_cast<float>(src_h) / static_cast<float>(effect_roi.height_);
    for(size_t i = 0; i < bboxes.size(); ++i){
        box_info &bbox = bboxes[i];
        bbox.x1_ = (bbox.x1_ - effect_roi.x_) * width_ratio;
        bbox.y1_ = (bbox.y1_ - effect_roi.y_) * height_ratio;
        bbox.x2_ = (bbox.x2_ - effect_roi.x_) * width_ratio;
        bbox.y2_ = (bbox.y2_ - effect_roi.y_) * height_ratio;
    }
}

void resize_aspect_ratio_to_height(cv::Mat const &src, cv::Mat &dst, int height)
{
    auto const ratio = height / static_cast<double>(src.rows);
    cv::resize(src, dst, cv::Size(static_cast<int>(src.cols * ratio), height));
}

void resize_aspect_ratio_to_width(cv::Mat const &src, cv::Mat &dst, int width)
{
    auto const ratio = width / static_cast<double>(src.cols);
    cv::resize(src, dst, cv::Size(width, static_cast<int>(src.rows * ratio)));
}

}
