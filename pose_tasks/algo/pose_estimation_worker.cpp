#include "pose_estimation_worker.hpp"

#include <cv_algo/pose/movenet_single_pose_estimate.hpp>
#include <cv_algo/pose/pose_estimation_utils.hpp>
#include <utils/qimage_to_cvmat.hpp>

#include "pose_estimation_worker_results.hpp"
#include "../config/config_pose_estimation_worker.hpp"

#include <QDebug>

#include <opencv2/imgproc.hpp>

using namespace flt::cvt::pose;

struct pose_estimation_worker::impl
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

    pose_estimation_worker_results process_results(QImage rgb)
    {        
        pose_estimation_worker_results results;
        if(config_.source_type_ == flt::mm::stream_source_type::image){
            //convert rgb to RGB888 may make the predict results weird
            results.qimg_ = std::any_cast<QImage>(rgb).convertToFormat(QImage::Format_RGB888);
            auto mat = cv::Mat(results.qimg_.height(), results.qimg_.width(), CV_8UC3, results.qimg_.bits(), results.qimg_.bytesPerLine());
            results.points_ = net_.predict(mat);
            flt::cvt::pose::draw(mat, results.points_, config_.confidence_);
        }else{
            auto [mat, non_copy] = flt::qimg_convert_to_cvmat_non_copy(rgb);
            cv::cvtColor(mat, mat, cv::COLOR_BGR2RGB);
            results.points_ = net_.predict(mat);
            flt::cvt::pose::draw(mat, results.points_, config_.confidence_);
            if(non_copy){
                results.qimg_ = rgb;
            }else{
                //Todo : reduce useless operations
                results.qimg_ = QImage(mat.data, mat.cols, mat.rows, mat.step[0], QImage::Format_RGB888).copy();
            }
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
