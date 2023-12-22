#pragma once

#include <opencv2/core.hpp>

#include <net.h>

#include <vector>

namespace flt::cvt::face{

struct anchor_cfg
{
    anchor_cfg(std::vector<float> s, std::vector<float> r, int size, float cls_threshold = 0.8f);

    int base_size_;
    float cls_threshold_;
    std::vector<float> ratios_;
    std::vector<float> scales_;
};

class cret2f
{
public:
    cret2f(float x1, float x2, float x3, float x4)
    {
        val[0] = x1;
        val[1] = x2;
        val[2] = x3;
        val[3] = x4;

    }

    float operator[](int i) const
    {
        return val[i];
    }

    float& operator[](int i)
    {
        return val[i];
    }

    float val[4];

    void print()
    {
        printf("rect %f %f %f %f\n", val[0], val[1], val[2], val[3]);
    }
};

class anchor
{
public:
    anchor();

    bool operator<(const anchor &t) const noexcept
    {
        return score_ < t.score_;
    }

    bool operator>(const anchor &t) const noexcept
    {
        return score_ > t.score_;
    }    

    float operator[](int i) const noexcept
    {
        switch(i){
        case 0:
            return final_box_.x;
        case 1:
            return final_box_.y;
        case 2:
            return final_box_.width;
        case 3:
            return final_box_.height;
        default:
            return - 1;
        }
    }

    void print();

    cv::Rect_<float> final_box_; //use to save finalbox (after adding the delta)
    cv::Rect_<float> anchor_;  // use to save orignal anchor
    float score_;              // use to save the anchor score
    std::vector<cv::Point2f> pts_;  // use to save the landmark point
    cv::Point center_;    
};


class face_detector_anchor_creator
{
public:
    size_t init(int stride, anchor_cfg const &cfg, bool dense_anchor);
    void filter_anchor(ncnn::Mat& cls, ncnn::Mat& reg, ncnn::Mat& pts, std::vector<anchor>& proposals) const;

private:
    void box_pred(cret2f const &per_anc, cret2f const &delta, cv::Rect_<float> &box) const;
    void landmark_pred(cret2f const &box, std::vector<cv::Point2f> const &pts_delta, std::vector<cv::Point2f>& landmark_pre) const;
    void ratio_enum(cret2f const &base_anchor, std::vector<float> const &ratio, std::vector<cret2f> &ratio_anchor) const;
    void scale_enum(std::vector<cret2f>const &ratio_anchor, std::vector<float> const &scales, std::vector<cret2f> &scale_anchor) const;

    std::vector<cret2f> pre_anchor_;
    int anchor_stride_;
    int anchor_num_;

    float cls_threshold_;
};

}
