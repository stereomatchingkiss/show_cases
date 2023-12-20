#include "pptsm_v2_worker.hpp"

#include "../../config/config_alert_sender.hpp"
#include "../../config/config_pptsm_v2_worker.hpp"
#include "../generic_worker_results.hpp"

#include "action_classify_alert_save.hpp"
#include "kinetic_400_labels.hpp"

#include <cv_algo/converter/qt_and_cv_rect_converter.hpp>
#include <cv_algo/video/video_classify_pptsm_opencv.hpp>
#include <utils/qimage_to_cvmat.hpp>

#ifdef WASM_BUILD

#include <utils/image_utils.hpp>

#endif

#include <opencv2/imgproc.hpp>

#include <QDebug>
#include <QElapsedTimer>
#include <QPainter>

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

#include <format>

using namespace flt::cvt;
using namespace flt::cvt::video;
using namespace flt::mm;

struct pptsm_v2_worker::impl
{
    impl(config_ppstm_v2_worker const &config) :
        config_{config},
        labels_{kinetic_400_labels::get_labels()},
        net_{create_model_root() + "ppTSMv2_8f_simple.onnx", config.config_action_classify_model_select_.sampling_rate_, 8, true}
    {
        pen_.setColor(Qt::red);
        pen_.setWidth(5);

        change_alert_sender_config(config_.config_alert_sender_);
    }

    void change_alert_sender_config(config_alert_sender const &val)
    {
        alert_save_.change_alert_sender_config(val);
    }

    std::string create_model_root() const
    {
#ifndef WASM_BUILD
        return ("assets/action_classify/");
#else
        return "";
#endif
    }    

    QString create_fname() const
    {
        return  QString("%1").arg(QDateTime::currentDateTime().toString("hh_mm_ss"));
    }

    cv::Mat to_cv_mat(QPainter &painter, QImage &qimg)
    {
        cv::Mat mat;
        if(!config_.roi_.isEmpty()){
            auto const scaled_roi = convert_qrectf_to_cv_rect(config_.roi_, qimg.width(), qimg.height());
            auto const rect = QRect(scaled_roi.x, scaled_roi.y, scaled_roi.width, scaled_roi.height);

            painter.setPen(pen_);
            painter.drawRect(rect);

            auto copy_qimg = qimg.copy(rect);
            return std::get<0>(flt::qimg_convert_to_cvmat_non_copy(copy_qimg));
        }

        return std::get<0>(flt::qimg_convert_to_cvmat_non_copy(qimg));
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
        if(!results.empty()){
            if(std::get<0>(results[0]) < config_.config_action_classify_model_select_.confidence_){
                results.clear();
            }
        }

        auto iters = std::ranges::remove_if(results, [this](auto const &val)
                                            {
                                                return config_.config_select_action_to_classify_.selected_object_[std::get<1>(val)] == false;
                                            });
        results.erase(iters.begin(), iters.end());

        return results;
    }    

    bool should_emit_alarm() const noexcept
    {
        if(duration_of_detected_label_.isValid() && config_.config_action_classify_alert_.alert_if_action_detected_on_){
            return duration_of_detected_label_.elapsed() >
                   config_.config_action_classify_alert_.alert_if_action_detected_duration_sec_ * 1000;
        }

        return false;
    }

    void update_duration(bool result_is_empty)
    {
        if(!result_is_empty){
            if(!duration_of_detected_label_.isValid()){
                duration_of_detected_label_.start();
            }
        }else{
            duration_of_detected_label_.invalidate();
        }
    }

    action_classify_alert_save alert_save_;
    config_ppstm_v2_worker config_;
    QElapsedTimer duration_of_detected_label_;
    std::vector<QString> labels_;
    video_classify_pptsm_opencv net_;
    QPen pen_;    
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
    impl_->change_alert_sender_config(val);
}

void pptsm_v2_worker::process_results(std::any frame)
{
    auto qimg = std::any_cast<QImage>(frame);
    QPainter painter(&qimg);

    auto const results = impl_->predict(impl_->to_cv_mat(painter, qimg));
    if(!results.empty()){
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

        impl_->update_duration(false);
    }else{
        impl_->update_duration(true);
    }

    generic_worker_results output;
    output.alarm_on_ = impl_->should_emit_alarm();
    //do not move it, since in the future this algo may need to support multi-stream
    output.mat_ = qimg;
    if(output.alarm_on_){
        impl_->alert_save_.save_to_json(results, qimg, impl_->duration_of_detected_label_.elapsed());
        if(impl_->alert_save_.send_alert()){
            if(impl_->alert_save_.send_by_text()){
                emit send_alert_by_text(impl_->alert_save_.get_alert_info());
            }else{
                emit send_alert_by_binary(impl_->alert_save_.get_alert_info());
            }
        }
    }

    emit send_process_results(std::move(output));
}
