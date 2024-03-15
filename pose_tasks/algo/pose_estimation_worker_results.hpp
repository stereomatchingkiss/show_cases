#pragma once

#include <cv_algo/pose/pose_estimation_results.hpp>

#include <QImage>

struct pose_estimation_worker_results
{
    QImage qimg_;

    std::vector<flt::cvt::pose::keypoint> points_;
};
