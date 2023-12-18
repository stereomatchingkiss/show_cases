#include "pptsm_v2_worker.hpp"

#include "../../config/config_alert_sender.hpp"
#include "../../config/config_pptsm_v2_worker.hpp"
#include "../generic_worker_results.hpp"

#include "kinetic_400_labels.hpp"

#include <cv_algo/converter/qt_and_cv_rect_converter.hpp>
#include <cv_algo/video/video_classify_pptsm_opencv.hpp>
#include <utils/qimage_to_cvmat.hpp>

#include <opencv2/imgproc.hpp>

#include <QDebug>
#include <QPainter>

#include <format>

using namespace flt::cvt;
using namespace flt::cvt::video;
using namespace flt::mm;

struct pptsm_v2_worker::impl
{
    impl(config_ppstm_v2_worker const &config) :
        config_{config},
        labels_{kinetic_400_labels::get_labels()},
        net_{create_model_root() + "ppTSMv2_8f_simple.onnx", config.config_action_classify_model_select_.sampling_rate_}
    {
        pen_.setColor(Qt::red);
        pen_.setWidth(5);
    }

    std::string create_model_root() const
    {
#ifndef WASM_BUILD
        return ("assets/action_classify/");
#else
        return "";
#endif
    }

    void print_debug_msg(auto const &results) const
    {
        QString debug_str;
        for(size_t i = 0; i != results.size(); ++i){
            auto const [confidence, label] = results[i];
            debug_str += std::format("{}:{:.2f},", labels_[label].toStdString(), confidence).c_str();
        }
        qDebug()<<debug_str;
    }

    auto predict(cv::Mat const &input)
    {        
        auto results = net_.predict(input, config_.config_action_classify_model_select_.top_k_);
        //print_debug_msg(results);
        auto iters = std::ranges::remove_if(results, [this](auto const &val)
                                            {
                                                return config_.config_select_action_to_classify_.selected_object_[std::get<1>(val)] == false;
                                            });
        results.erase(iters.begin(), iters.end());
        if(!results.empty()){
            if(std::get<0>(results[0]) < config_.config_action_classify_model_select_.confidence_){
                results.clear();
            }
        }

        return results;
    }

    config_ppstm_v2_worker config_;
    std::vector<QString> labels_;
    video_classify_pptsm_opencv net_;
    QPen pen_;
    cv::Rect scaled_roi_;
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

    QPainter painter(&qimg);
    cv::Mat mat;
    if(!impl_->config_.roi_.isEmpty()){
        impl_->scaled_roi_ = convert_qrectf_to_cv_rect(impl_->config_.roi_, qimg.width(), qimg.height());
        auto const rect = QRect(impl_->scaled_roi_.x, impl_->scaled_roi_.y, impl_->scaled_roi_.width, impl_->scaled_roi_.height);
        auto copy_qimg = qimg.copy(rect);
        mat = std::get<0>(flt::qimg_convert_to_cvmat_non_copy(copy_qimg));
        cv::cvtColor(mat, mat, cv::COLOR_RGB2BGR);

        painter.setPen(impl_->pen_);
        painter.drawRect(rect);
    }else{
        impl_->scaled_roi_ = cv::Rect(qimg.rect().x(), qimg.rect().y(), qimg.width(), qimg.height());
        mat = std::get<0>(flt::qimg_convert_to_cvmat_non_copy(qimg));
        cv::cvtColor(mat, mat, cv::COLOR_RGB2BGR);
    }

    if(auto const results = impl_->predict(mat); !results.empty()){
        QFont font;
        auto const font_size = qimg.width() / 20;
        font.setPixelSize(font_size);
        painter.setFont(font);
        painter.setPen(Qt::blue);

        for(int i = 0; i != results.size(); ++i){
            auto const [confidence, label] = results[i];
            painter.drawText(QPoint(0, (i + 1) * font_size),
                             std::format("{}:{:.2f}", impl_->labels_[label].toStdString(), confidence).c_str());
        }
    }

    generic_worker_results output;
    output.mat_ = std::move(qimg);

    emit send_process_results(std::move(output));
}
