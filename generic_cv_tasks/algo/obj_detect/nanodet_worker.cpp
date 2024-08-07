#include "nanodet_worker.hpp"

#include "nanodet_alert_save.hpp"

#include "../convert_input_to_image.hpp"
#include "../generic_obj_detector.hpp"
#include "../generic_worker_results.hpp"

#include "../../config/config_alert_sender.hpp"
#include "../../config/config_generic_obj_detector.hpp"
#include "../../config/config_nanodet_worker.hpp"

#include "../../global/global_keywords.hpp"

#include <cv_algo/converter/box_type_converter.hpp>
#include <cv_algo/converter/qt_and_cv_rect_converter.hpp>

#include <cv_algo/obj_detect/obj_det_utils.hpp>
#include <cv_algo/obj_detect/nanodet/nanodet.hpp>

#include <cv_algo/tracker/track_object_pass.hpp>
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
using namespace flt::cvt::tracker;

struct nanodet_worker::impl
{
    impl(config_nanodet_worker config) :
        alert_save_(global_keywords().coco_names()),
        config_{std::move(config)},
        names_{global_keywords().coco_names()}
    {
        config_generic_obj_detector gconfig;
        gconfig.config_object_detect_model_select_ = config_.config_object_detect_model_select_;
        gconfig.config_select_object_to_detect_ = config_.config_select_object_to_detect_;
        obj_det_ = std::make_unique<generic_obj_detector>(std::move(gconfig));

        change_alert_sender_config(config_.config_alert_sender_);
    }

    bool check_alarm_condition(track_results const &pass_results, QImage const &img)
    {
        bool alarm_on = false;
        if(config_.config_tracker_alert_.alert_if_stay_in_roi_on_){
            alert_save_.clear_im_name();
            for(auto const &val : pass_results.track_durations_){
                if(val.duration_sec_ >= config_.config_tracker_alert_.alert_if_stay_in_roi_duration_sec_ &&
                    !written_id_.contains(val.id_)){
                    alarm_on = true;
                    written_id_.insert(val.id_);
                    alert_save_.save_to_json(val, img);
                }
            }
        }

        return alarm_on;
    }

    void clear_written_id()
    {
        if(im_ids_ % 500 == 0){
            for(auto it = std::begin(written_id_); it != std::end(written_id_);){
                if(track_obj_pass_->id_is_under_track(*it)){
                    written_id_.erase(it++);
                }else{
                    ++it;
                }
            }
        }
    }    

    void change_alert_sender_config(const config_alert_sender &val)
    {        
        config_.config_alert_sender_ = val;
        alert_save_.change_alert_sender_config(val);
    }

    void draw_pass_results(cv::Mat &mat, track_results const &pass_results) const
    {
        cv::putText(mat, std::format("up:{}, down:{}", pass_results.count_top_pass_, pass_results.count_bottom_pass_),
                    cv::Point(0, 50), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(200, 255, 0), 4);
        cv::putText(mat, std::format("left:{}, right:{}", pass_results.count_left_pass_, pass_results.count_right_pass_),
                    cv::Point(0, 100), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(200, 255, 0), 4);
        cv::putText(mat, std::format("in the rect:{}", pass_results.count_in_center_),
                    cv::Point(0, 150), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(200, 255, 0), 4);
        if(!scaled_roi_.empty()){
            flt::cvt::utils::draw_empty_rect(mat, scaled_roi_);
        }
    }    

    auto track_obj(cv::Mat &mat)
    {       
        auto det_results = obj_det_->predict(mat);
        auto track_obj = box_info_to_byte_track_obj(det_results);
        auto const track_ptr_vec = tracker_.update(track_obj);

        det_results = byte_track_obj_to_box_info(track_ptr_vec);
        for(auto const &val : det_results){
            det::draw_bboxes_custom(mat, val, std::format("{}:{}", names_[val.label_], val.track_id_));
        }

        return det_results;
    }

    nanodet_alert_save alert_save_;    
    config_nanodet_worker config_;
    size_t im_ids_ = 0;
    std::vector<std::string> names_;
    std::unique_ptr<generic_obj_detector> obj_det_;
    cv::Rect scaled_roi_;
    BYTETracker tracker_;
    std::unique_ptr<track_object_pass> track_obj_pass_;
    std::set<int> written_id_;
};

nanodet_worker::nanodet_worker(config_nanodet_worker config, QObject *parent) :
    flt::mm::frame_process_base_worker(2, parent),
    impl_{std::make_unique<impl>(std::move(config))}
{

}

nanodet_worker::~nanodet_worker()
{

}

void nanodet_worker::change_alert_sender_config(const config_alert_sender &val)
{
    impl_->change_alert_sender_config(val);
}

void nanodet_worker::process_results(std::any frame)
{
    auto [mat, qimg] = convert_std_any_to_image(frame, impl_->config_.source_type_);

    if(!impl_->track_obj_pass_){
        if(impl_->config_.roi_.isValid()){
            impl_->scaled_roi_ = convert_qrectf_to_cv_rect(impl_->config_.roi_, mat.cols, mat.rows);
            impl_->track_obj_pass_ =
                std::make_unique<cvt::tracker::track_object_pass>(impl_->scaled_roi_, 30);
        }else{
            impl_->track_obj_pass_ =
                std::make_unique<cvt::tracker::track_object_pass>(cv::Rect(0, 0, mat.cols - 1, mat.rows - 1), 30);
        }
    }

    auto const det_results = impl_->track_obj(mat);        
    auto const pass_results = impl_->track_obj_pass_->track(det_results);
    impl_->draw_pass_results(mat, pass_results);

    generic_worker_results results;
    if(impl_->check_alarm_condition(pass_results, qimg)){
        results.alarm_on_ = true;
        ++impl_->im_ids_;
        impl_->clear_written_id();
        if(impl_->alert_save_.send_alert()){
            if(impl_->alert_save_.send_by_text()){
                emit send_alert_by_text(impl_->alert_save_.get_alert_info());
            }else{
                emit send_alert_by_binary(impl_->alert_save_.get_alert_info());
            }
        }
    }

    if(impl_->config_.source_type_ != flt::mm::stream_source_type::rtsp){
        results.mat_ = std::move(qimg);
    }else{
        results.mat_ = qimg.copy();
    }

    emit send_process_results(std::move(results));
}
