#include "image_process.hpp"

#include <opencv2/imgproc.hpp>

#include <algorithm>

namespace ocv::utils{

void adjust_img_orientation(const cv::Mat &input, cv::Mat &output, int rotate_angle, bool horizontal_flip)
{
    if(rotate_angle != 0){
        rotate_90n(input, output, rotate_angle);
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
