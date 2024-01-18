#include "aruco_detect_worker.hpp"

#include "../config/config_aruco_detect_worker.hpp"

#include "aruco_detect_worker_results.hpp"

#include <QDebug>

#include <opencv2/aruco.hpp>
#include <opencv2/imgproc.hpp>

#include <mutex>

struct aruco_detect_worker::impl
{
    impl(config_aruco_detect_worker const &config) :
        detector_params_{config.detector_params_},
        dictionary_{config.dictionary_}
    {
        det_.setDictionary(dictionary_);
        det_.setDetectorParameters(detector_params_);
    }

    aruco_detect_worker_results process_results(QImage rgb)
    {

        aruco_detect_worker_results results;
        results.qimg_ = std::any_cast<QImage>(rgb).convertToFormat(QImage::Format_RGB888);
        auto mat = cv::Mat(results.qimg_.height(), results.qimg_.width(), CV_8UC3, results.qimg_.bits(), results.qimg_.bytesPerLine());

        cv::cvtColor(mat, bgr_, cv::COLOR_RGB2BGR);
        {
            std::lock_guard<std::mutex> lock(mutex_);
            det_.detectMarkers(bgr_, results.marker_corners_, results.marker_ids_, results.rejected_candidates_);
        }

        cv::aruco::drawDetectedMarkers(mat, results.marker_corners_, results.marker_ids_);

        return results;
    }

    cv::Mat bgr_;

    cv::aruco::ArucoDetector det_;
    cv::aruco::DetectorParameters detector_params_;
    cv::aruco::Dictionary dictionary_;

    std::mutex mutex_;
};

aruco_detect_worker::aruco_detect_worker(config_aruco_detect_worker const &config, QObject *parent) :
    flt::mm::frame_process_base_worker(2, parent),
    impl_{std::make_unique<impl>(config)}
{

}

aruco_detect_worker::~aruco_detect_worker()
{

}

void aruco_detect_worker::process_results(std::any frame)
{        
    emit send_process_results(impl_->process_results(std::any_cast<QImage>(frame)));
}
