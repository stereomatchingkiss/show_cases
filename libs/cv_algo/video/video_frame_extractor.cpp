#include "video_frame_extractor.hpp"

#include <opencv2/dnn.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include <iostream>
#include <format>

namespace flt::cvt::video{

video_frame_extractor::video_frame_extractor(int sampling_rate_, int num_seg) :
    center_idx_{(sampling_rate_ / 2) - 1},
    frame_count_{0},    
    num_seg_{num_seg},
    sampling_rate_{std::max(sampling_rate_, 1)}
{

}

bool video_frame_extractor::enough_frames() const noexcept
{
    return input_blobs_.size() == num_seg_;
}

void video_frame_extractor::extract(const cv::Mat &bgr)
{
    if(sampling_rate_ > 1){
        if((frame_count_ % sampling_rate_) == center_idx_){
            input_blobs_.emplace_back(preprocess(bgr));
        }
    }else{
        input_blobs_.emplace_back(preprocess(bgr));
    }

    if(input_blobs_.size() > num_seg_){
        input_blobs_.erase(std::begin(input_blobs_));
    }

    ++frame_count_;
}

const std::vector<cv::Mat> &video_frame_extractor::get_blobs() const noexcept
{
    return input_blobs_;
}

const cv::Mat &video_frame_extractor::get_output_blob()
{
    cv::dnn::blobFromImages(input_blobs_, output_blob_);

    return output_blob_;
}

int video_frame_extractor::get_target_size() const noexcept
{
    return 224;
}

int video_frame_extractor::get_frames_per_batch() const noexcept
{
    return sampling_rate_;
}

void video_frame_extractor::center_crop(cv::Mat const &input, cv::Mat &crop_img, int target_size) const
{
    int const h = input.rows;
    int const w = input.cols;
    if(w < target_size || h < target_size){
        cv::resize(input, crop_img, cv::Size(target_size, target_size));
    }else{
        int const x1 = (w - target_size) / 2;
        int const y1 = (h - target_size) / 2;
        crop_img = input(cv::Rect(x1, y1, target_size, target_size));
    }
}

void video_frame_extractor::normalize(cv::Mat const &input, cv::Mat &output) const
{
    int const channels = input.channels();
    for(int i = 0; i != channels; ++i){
        for(int row = 0; row != input.rows; ++row){
            auto input_ptr = input.ptr<uchar>(row);
            auto output_ptr = output.ptr<float>(row);
            for(int col = 0; col < input.cols * 3; col += 3){
                output_ptr[col + (channels - i - 1)] = ((input_ptr[col + i] / 255.0f) - mean_[i]) / scale_[i];
            }
        }
    }
}

cv::Mat video_frame_extractor::preprocess(const cv::Mat &input) const
{
    cv::Mat crop_img;
    center_crop(resize(input), crop_img, get_target_size());

    cv::Mat blob(crop_img.size(), CV_32FC3);
    normalize(crop_img, blob);

    return blob;
}

void video_frame_extractor::resize(cv::Mat const &input, cv::Mat &resize_img, int short_size) const
{
    int const h = input.rows;
    int const w = input.cols;
    if((w <= h && w == short_size) || (h <= w && h == short_size)){
        input.copyTo(resize_img);
    }else{
        int oh, ow;
        if (w < h){
            ow = short_size;
            oh = h * ow / w;
        }else{
            oh = short_size;
            ow = w * oh / h;
        }
        cv::resize(input, resize_img, cv::Size(ow, oh), 0.0f, 0.0f, cv::INTER_LINEAR);
    }
}

cv::Mat video_frame_extractor::resize(const cv::Mat &input) const
{
    cv::Mat resize_img;
    resize(input, resize_img, 256);

    return resize_img;
}



}
