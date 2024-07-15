#include "fall_down_obj_det_worker.hpp"

#include "../../config/config_alert_sender.hpp"
#include "../../config/config_fall_down_obj_det_worker.hpp"
#include "../../config/config_generic_obj_detector.hpp"

#include "../../global/global_keywords.hpp"

#include "../generic_obj_detector.hpp"
#include "../generic_worker_results.hpp"

#include "fall_down_det_alert_save.hpp"

#include <cv_algo/converter/box_type_converter.hpp>
#include <cv_algo/converter/qt_and_cv_rect_converter.hpp>

#include <cv_algo/obj_detect/obj_det_utils.hpp>
#include <cv_algo/obj_detect/nanodet/nanodet.hpp>

#include <cv_algo/tracker/track_results.hpp>
#include <cv_algo/tracker/byte_track/BYTETracker.hpp>

#include <cv_algo/utils/draw_rect.hpp>

#include <utils/image_utils.hpp>

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>

#include <QDebug>
#include <QRectF>

#include <format>
#include <mutex>

using namespace flt;
using namespace flt::cvt;
using namespace flt::cvt::det;
using namespace flt::cvt::tracker;

struct fall_down_obj_det_worker::impl
{
    impl(config_fall_down_obj_det_worker const &config) :
        alert_save_(global_keywords().coco_names()),
        config_{config},
        names_{global_keywords().coco_names()},
        tracker_{30,
                 30,
                 config_.config_object_detect_model_select_.confidence_,
                 std::min(config_.config_object_detect_model_select_.confidence_ + 0.05f, 0.8f),
                 0.8f}
    {
        config_generic_obj_detector gconfig;
        gconfig.config_object_detect_model_select_ = config_.config_object_detect_model_select_;
        gconfig.config_select_object_to_detect_.selected_object_.insert("person");
        obj_det_ = std::make_unique<generic_obj_detector>(std::move(gconfig));
    }

    void change_alert_sender_config(const config_alert_sender &val)
    {
        config_.config_alert_sender_ = val;
    }

    bool save_alert_info(QImage const &img)
    {
        bool can_send_alert = false;
        if(can_send_alert_ && !fall_down_counter_.empty()){
            using wt = fall_down_warning_type;
            for(auto it = std::begin(fall_down_counter_); it != std::end(fall_down_counter_); ++it){
                qDebug()<<"alert id = "<<it->first;
                if(it->second.can_issued_){
                    qDebug()<<__func__<<":0";
                    if(config_.config_fall_down_obj_det_alert_.warning_type_ == wt::issue_a_warning &&
                        it->second.warning_issued_ == false){
                        qDebug()<<__func__<<":1";
                        can_send_alert = true;
                    }else if(config_.config_fall_down_obj_det_alert_.warning_type_ == wt::issue_warning_periodic){
                        qDebug()<<__func__<<":2";
                        can_send_alert = true;
                    }

                    qDebug()<<__func__<<":3";
                    it->second.warning_issued_ = true;
                }
            }
        }

        if(can_send_alert){
            alert_save_.save_to_json(img);
        }

        return can_send_alert;
    }

    bool update_fall_down_counter(int id)
    {
        active_id_.insert(id);
        if(auto it = fall_down_counter_.find(id); it != std::end(fall_down_counter_)){
            ++it->second.continuous_active_;
            it->second.continuous_non_active_ = 0;

            if(it->second.continuous_active_ >= config_.config_fall_down_condition_.number_of_consecutive_falls_){
                it->second.can_issued_ = true;
                it->second.continuous_active_ = 0;                
                return true;
            }
        }else{
            fall_down_counter_.insert({id, {}});

            if(1 >= config_.config_fall_down_condition_.number_of_consecutive_falls_){
                it->second.can_issued_ = true;
                return true;
            }
        }

        return false;
    }

    void remove_id_lost_track()
    {
        for(auto it = std::begin(fall_down_counter_); it != std::end(fall_down_counter_);){
            if(!active_id_.contains(it->first)){
                ++it->second.continuous_non_active_;
                if(it->second.continuous_non_active_ > lost_track_threshold_){
                    fall_down_counter_.erase(it++);
                }else{
                    ++it;
                }
            }else{
                ++it;
            }
        }
    }

    auto track_obj(cv::Mat &mat)
    {        
        auto det_results = obj_det_->predict(mat);
        auto track_obj = box_info_to_byte_track_obj(det_results);
        auto const track_ptr_vec = tracker_.update(track_obj);

        det_results = byte_track_obj_to_box_info(track_ptr_vec);
        active_id_.clear();
        can_send_alert_ = false;
        for(auto const &val : det_results){
            det::draw_bboxes_custom(mat, val, std::format("{}:{}", names_[val.label_], val.track_id_));
            auto const wh_ratio = width_height_ratio(val);            
            if(wh_ratio >= config_.config_fall_down_condition_.width_height_ratio_){
                if(update_fall_down_counter(val.track_id_)){
                    can_send_alert_ = true;
                }
                det::draw_bboxes_custom(mat, val, std::format("{}:{}:fall", names_[val.label_], val.track_id_));
            }else{
                det::draw_bboxes_custom(mat, val, std::format("{}:{}", names_[val.label_], val.track_id_));
            }
        }                

        remove_id_lost_track();

        return det_results;
    }

    float width_height_ratio(box_info const &val) const noexcept
    {
        auto const tl = val.tl();
        auto const br = val.br();
        auto const width = br.x - tl.x;
        auto const height = br.y - tl.y;

        return std::abs(width/(height + 0.001f));
    }

    struct fall_down_count
    {
        bool can_issued_ = false;
        int continuous_active_ = 1;
        int continuous_non_active_ = 0;
        bool warning_issued_ = false;
    };

    std::set<int> active_id_;
    fall_down_det_alert_save alert_save_;
    bool can_send_alert_ = false;
    config_fall_down_obj_det_worker config_;
    std::map<int, fall_down_count> fall_down_counter_;
    static int constexpr lost_track_threshold_ = 300;
    std::vector<std::string> names_;
    std::unique_ptr<generic_obj_detector> obj_det_;
    cv::Rect scaled_roi_;
    BYTETracker tracker_;
};

fall_down_obj_det_worker::fall_down_obj_det_worker(const config_fall_down_obj_det_worker &config, QObject *parent) :
    flt::mm::frame_process_base_worker(2, parent),
    impl_{std::make_unique<impl>(std::move(config))}
{

}

fall_down_obj_det_worker::~fall_down_obj_det_worker()
{

}

void fall_down_obj_det_worker::change_alert_sender_config(const config_alert_sender &val)
{
    impl_->change_alert_sender_config(val);
}

void fall_down_obj_det_worker::process_results(std::any frame)
{
    auto qimg = std::any_cast<QImage>(frame).convertToFormat(QImage::Format_RGB888);
    auto mat = cv::Mat(qimg.height(), qimg.width(), CV_8UC3, qimg.bits(), qimg.bytesPerLine());

    auto const det_results = impl_->track_obj(mat);
    generic_worker_results results;
    results.alarm_on_ = impl_->can_send_alert_;

    if(impl_->config_.roi_.isValid()){
        impl_->scaled_roi_ = convert_qrectf_to_cv_rect(impl_->config_.roi_, mat.cols, mat.rows);
        if(!impl_->scaled_roi_.empty()){
            flt::cvt::utils::draw_empty_rect(mat, impl_->scaled_roi_);
        }
    }

    impl_->save_alert_info(qimg);

    //do not move it, since in the future this algo may need to support multi-stream
    results.mat_ = qimg;

    emit send_process_results(std::move(results));//*/
}
