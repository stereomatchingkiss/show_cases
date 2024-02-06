#include "aruco_stitch_worker.hpp"

#include "../config/config_aruco_stitch_worker.hpp"

#include <opencv2/aruco.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/stitching.hpp>

struct aruco_stitch_worker::impl
{
    impl(config_aruco_stitch_worker const &config) :
        detector_params_{config.detector_params_},
        dictionary_{config.dictionary_}
    {
        det_.setDictionary(dictionary_);
        det_.setDetectorParameters(detector_params_);
    }

    /*aruco_detect_worker_results process_results(QImage rgb)
    {

        aruco_detect_worker_results results;
        results.qimg_ = std::any_cast<QImage>(rgb).convertToFormat(QImage::Format_RGB888);
        if(results.qimg_.width() > 640){
            results.qimg_ = results.qimg_.scaledToWidth(640, Qt::SmoothTransformation);
        }
        auto mat = cv::Mat(results.qimg_.height(), results.qimg_.width(), CV_8UC3, results.qimg_.bits(), results.qimg_.bytesPerLine());

        cv::cvtColor(mat, bgr_, cv::COLOR_RGB2BGR);
        det_.detectMarkers(bgr_, results.marker_corners_, results.marker_ids_, results.rejected_candidates_);

        cv::aruco::drawDetectedMarkers(mat, results.marker_corners_, results.marker_ids_);

        return results;
    }//*/

    cv::Mat bgr_;

    cv::aruco::ArucoDetector det_;
    cv::aruco::DetectorParameters detector_params_;
    cv::aruco::Dictionary dictionary_;

    std::set<int> aruco_id_detected_;
};

aruco_stitch_worker::aruco_stitch_worker(const config_aruco_stitch_worker &config, QObject *parent) :
    flt::mm::frame_process_base_worker(2, parent),
    impl_{std::make_unique<impl>(config)}
{

}

aruco_stitch_worker::~aruco_stitch_worker()
{

}

void aruco_stitch_worker::process_results(std::any frame)
{

}
