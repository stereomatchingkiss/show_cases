#include "pose_estimation_worker.hpp"

#include <cv_algo/pose/movenet_single_pose_estimate.hpp>
#include <cv_algo/pose/pose_estimation_utils.hpp>
#include <utils/qimage_to_cvmat.hpp>

#include "pose_estimation_common_func.hpp"
#include "pose_estimation_worker_results.hpp"
#include "../config/config_pose_estimation_worker.hpp"

#include <QDebug>

#include <QJsonDocument>
#include <QJsonObject>

#include <opencv2/imgproc.hpp>

#include <format>

using namespace flt::cvt::pose;

struct pose_estimation_worker::impl
{
    impl(config_psoe_estimation_worker const &config) :
        config_{config},
        net_{pose_param_path(), pose_bin_path(), 256, false}
    {

    }

    pose_estimation_worker_results process_results(QImage rgb)
    {        
        auto results = predict_pose<pose_estimation_worker_results>(rgb, config_.confidence_, net_);
        if(config_.source_type_ == flt::mm::stream_source_type::websocket){
            results.json_text_ = QJsonDocument(convert_to_json(results.points_)).toJson(QJsonDocument::Compact);
        }

        return results;
    }

    config_psoe_estimation_worker const config_;

    movenet_single_pose_estimate net_;
};

pose_estimation_worker::pose_estimation_worker(config_psoe_estimation_worker const &config, QObject *parent) :
    flt::mm::frame_process_base_worker(2, parent),
    impl_{std::make_unique<impl>(config)}
{

}

pose_estimation_worker::~pose_estimation_worker()
{

}

void pose_estimation_worker::process_results(std::any frame)
{
    emit send_process_results(impl_->process_results(std::any_cast<QImage>(frame)));
}
