#pragma once

#include <opencv2/core.hpp>

namespace flt::cvt::face{

class face_warp
{
public:
    explicit face_warp(int target_size = 112);
    ~face_warp();

    cv::Mat process(cv::Mat const &input, std::vector<cv::Point2f> const &landmarks) const;

private:
    cv::Mat element_wise_minus(cv::Mat const &A,cv::Mat const &B) const;
    int matrix_rank(cv::Mat const &M) const;
    cv::Mat mean_axis_0(cv::Mat const &src) const;
    cv::Mat similar_transform(cv::Mat const &src,cv::Mat const &dst) const;
    cv::Mat var_axis_0(cv::Mat const &src) const;

    std::vector<cv::Point2f> target_warp_points_;
    cv::Mat target_face_landmarks_;

    int target_size_;
};

}
