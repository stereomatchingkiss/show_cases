#pragma once

#include <opencv2/aruco.hpp>

struct config_aruco_worker
{
    cv::aruco::DetectorParameters detector_params_;
    cv::aruco::Dictionary dictionary_;
};
