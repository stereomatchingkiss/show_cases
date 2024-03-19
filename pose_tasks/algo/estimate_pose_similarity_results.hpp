#pragma once

#include <cv_algo/pose/pose_estimation_results.hpp>

#include <QImage>

struct estimate_pose_similarity_results
{
    bool is_target_ = false;
    QImage qimg_;
    std::vector<flt::cvt::pose::keypoint> points_;

    float im_similarity_ = -1.0f;
};
