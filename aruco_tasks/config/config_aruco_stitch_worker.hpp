#ifndef CONFIG_ARUCO_STITCH_WORKER_HPP
#define CONFIG_ARUCO_STITCH_WORKER_HPP

#include <opencv2/aruco.hpp>
#include <opencv2/stitching.hpp>

struct config_aruco_stitch_worker
{
    cv::aruco::DetectorParameters detector_params_;
    cv::aruco::Dictionary dictionary_;

    cv::Stitcher::Mode stitch_mode_;
};

#endif // CONFIG_ARUCO_STITCH_WORKER_HPP
