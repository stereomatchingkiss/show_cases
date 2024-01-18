#pragma once

#include <QImage>

#include <opencv2/core.hpp>

struct aruco_detect_worker_results
{       
    std::vector<int> marker_ids_;
    std::vector<std::vector<cv::Point2f>> marker_corners_, rejected_candidates_;

    QImage qimg_;
};
