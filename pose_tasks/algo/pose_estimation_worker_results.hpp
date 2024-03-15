#pragma once

#include <cv_algo/pose/pose_estimation_results.hpp>

#include <QImage>

struct pose_estimation_worker_results
{
    QString json_text_;
    std::vector<flt::cvt::pose::keypoint> points_;
    QImage qimg_;
};
