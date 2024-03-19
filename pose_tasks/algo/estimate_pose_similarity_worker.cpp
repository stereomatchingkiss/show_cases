#include "estimate_pose_similarity_worker.hpp"

#include <cv_algo/pose/movenet_single_pose_estimate.hpp>
#include <cv_algo/pose/pose_estimation_utils.hpp>
#include <cv_algo/pose/pose_similarity_estimation.hpp>
#include <utils/qimage_to_cvmat.hpp>

#include "estimate_pose_similarity_worker_input.hpp"
#include "estimate_pose_similarity_worker_results.hpp"

#include "../config/config_pose_estimation_worker.hpp"

#include <QDebug>

#include <opencv2/imgproc.hpp>

using namespace flt::cvt::pose;

using stype = flt::mm::stream_source_type;

struct estimate_pose_similarity_worker::impl
{
    impl(config_psoe_estimation_worker const &config) :
        config_{config},
        net_{param_path(), bin_path(), 256, false}
    {

    }

    std::string model_root() const
    {
#ifndef WASM_BUILD
        return "assets/";
#else
        return "";
#endif
    }

    std::string bin_path() const
    {
        return model_root() + "thunder.bin";
    }

    std::string param_path() const
    {
        return model_root() + "thunder.param";
    }

    estimate_pose_similarity_worker_results predict(estimate_pose_similarity_worker_input &input)
    {
        auto [mat, non_copy] = flt::qimg_convert_to_cvmat_non_copy(input.qimg_);
        cv::cvtColor(mat, mat, cv::COLOR_BGR2RGB);

        estimate_pose_similarity_worker_results results;
        results.is_target_ = input.is_target_;
        results.points_ = net_.predict(mat);
        flt::cvt::pose::draw(mat, results.points_, config_.confidence_);
        if(non_copy){
            results.qimg_ = input.qimg_;
        }else{
            //Todo : reduce useless operations
            results.qimg_ = QImage(mat.data, mat.cols, mat.rows, mat.step[0], QImage::Format_RGB888).copy();
        }

        return results;
    }

    estimate_pose_similarity_worker_results process_image(estimate_pose_similarity_worker_input &input)
    {
        estimate_pose_similarity_worker_results results = predict(input);
        if(input.is_target_){
            source_points_ = std::move(results.points_);
        }else{
            target_points_ = std::move(results.points_);
        }
        if(!source_points_.empty() && !target_points_.empty()){
            results.im_similarity_ = euclidean_distance(source_points_, target_points_);
        }

        return results;
    }

    config_psoe_estimation_worker const config_;

    movenet_single_pose_estimate net_;

    std::vector<keypoint> source_points_;
    std::vector<keypoint> target_points_;
};

estimate_pose_similarity_worker::estimate_pose_similarity_worker(config_psoe_estimation_worker const &config, QObject *parent) :
    flt::mm::frame_process_base_worker(2, parent),
    impl_{std::make_unique<impl>(config)}
{

}

estimate_pose_similarity_worker::~estimate_pose_similarity_worker()
{

}

void estimate_pose_similarity_worker::process_results(std::any input)
{
    auto data = std::any_cast<estimate_pose_similarity_worker_input>(input);
    if(impl_->config_.source_type_ == stype::image){
        emit send_process_results(impl_->process_image(data));
    }
}
