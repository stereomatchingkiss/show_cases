#pragma once

#include <cv_algo/pose/pose_estimation_results.hpp>

#include <QImage>

struct estimate_many_pose_similarity_worker_results
{
    QString im_path_;
    QImage qimg_;
    std::vector<flt::cvt::pose::keypoint> points_;
};
