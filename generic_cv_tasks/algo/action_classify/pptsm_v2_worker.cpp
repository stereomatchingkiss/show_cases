#include "pptsm_v2_worker.hpp"

#include "../../config/config_alert_sender.hpp"
#include "../../config/config_pptsm_v2_worker.hpp"
#include "../generic_worker_results.hpp"

#include "kinetic_400_labels.hpp"

#include <cv_algo/video/video_classify_pptsm_opencv.hpp>

#include <opencv2/imgproc.hpp>

#include <QDebug>

#include <format>

using namespace flt::cvt::video;
using namespace flt::mm;

struct pptsm_v2_worker::impl
{
    impl(config_ppstm_v2_worker const &config) :
        config_{config},
        labels_{kinetic_400_labels::get_labels()},
        net_{create_model_root() + "ppTSMv2_8f_simple.onnx", config.config_action_classify_model_select_.sampling_rate_}
    {

    }

    std::string create_model_root() const
    {
#ifndef WASM_BUILD
        return ("assets/action_classify/");
#else
        return "";
#endif
    }

    auto predict(cv::Mat const &input)
    {
        auto results = net_.predict(input, config_.config_action_classify_model_select_.top_k_);
        if(!results.empty()){
            qDebug()<<std::get<0>(results[0])<<","<<labels_[std::get<1>(results[0])];
        }
        auto iters = std::ranges::remove_if(results, [this](auto const &val)
                               {
            return std::get<0>(val) < config_.config_action_classify_model_select_.confidence_ ||
                   config_.config_select_action_to_classify_.selected_object_[std::get<1>(val)] == false;
        });
        results.erase(iters.begin(), iters.end());

        return results;
    }

    config_ppstm_v2_worker config_;
    std::vector<QString> labels_;
    video_classify_pptsm_opencv net_;
};

pptsm_v2_worker::pptsm_v2_worker(config_ppstm_v2_worker const &config, QObject *parent) :
    frame_process_base_worker(2, parent),
    impl_(std::make_unique<impl>(config))
{

}

pptsm_v2_worker::~pptsm_v2_worker()
{

}

void pptsm_v2_worker::change_alert_sender_config(config_alert_sender const &val)
{
    //this is not a thread safe operation, but this algorithm do not need to worry about this issue
    impl_->config_.config_alert_sender_ = val;
}

void pptsm_v2_worker::process_results(std::any frame)
{
    auto qimg = std::any_cast<QImage>(frame);
    auto mat = cv::Mat(qimg.height(), qimg.width(), CV_8UC3, qimg.bits(), qimg.bytesPerLine());
    cv::cvtColor(mat, mat, cv::COLOR_RGB2BGR);

    if(auto const results = impl_->predict(mat); !results.empty()){
        for(int i = 0; i != results.size(); ++i){
            float confidence;
            int label;
            std::tie(confidence, label) = results[i];
            cv::putText(mat, std::format("{}:{:.2f}", impl_->labels_[label].toStdString(), confidence),
                        cv::Point(0, (i + 1) * 50), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(200, 255, 0), 4);
        }
    }

    generic_worker_results output;
    output.mat_ = std::move(qimg);

    emit send_process_results(std::move(output));
}
