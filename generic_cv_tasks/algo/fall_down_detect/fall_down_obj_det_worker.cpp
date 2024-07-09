#include "fall_down_obj_det_worker.hpp"

#include "../../config/config_alert_sender.hpp"
#include "../../config/config_fall_down_obj_det_worker.hpp"
#include "../../config/config_generic_obj_detector.hpp"

#include "../../global/global_keywords.hpp"

#include "../generic_obj_detector.hpp"
#include "../generic_worker_results.hpp"

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

    bool check_alarm_condition(track_results const &pass_results, QImage const &img)
    {
        bool alarm_on = false;
        /*if(config_.config_tracker_alert_.alert_if_stay_in_roi_on_){
            for(auto const &val : pass_results.track_durations_){
                if(val.duration_sec_ >= config_.config_tracker_alert_.alert_if_stay_in_roi_duration_sec_ &&
                    !written_id_.contains(val.id_)){
                    alarm_on = true;
                    written_id_.insert(val.id_);
                }
            }
        }//*/

        return alarm_on;
    }    

    void change_alert_sender_config(const config_alert_sender &val)
    {
        config_.config_alert_sender_ = val;
    }        

    float width_height_ratio(box_info const &val) const noexcept
    {
        auto const tl = val.tl();
        auto const br = val.br();
        //qDebug()<<std::format("{}:{},{}:{}", tl.x, tl.y, br.x, br.y).c_str();
        auto const width = br.x - tl.x;
        auto const height = br.y - tl.y;

        return std::abs(width/(height + 0.001f));
    }

    auto track_obj(cv::Mat &mat)
    {        
        auto det_results = obj_det_->predict(mat);
        auto track_obj = box_info_to_byte_track_obj(det_results);
        auto const track_ptr_vec = tracker_.update(track_obj);

        det_results = byte_track_obj_to_box_info(track_ptr_vec);
        for(auto const &val : det_results){
            det::draw_bboxes_custom(mat, val, std::format("{}:{}", names_[val.label_], val.track_id_));
            auto const wh_ratio = width_height_ratio(val);
            qDebug()<<"wh ratio = "<<wh_ratio<<", "<<config_.width_height_ratio_;
            if(wh_ratio >= config_.width_height_ratio_){
                det::draw_bboxes_custom(mat, val, std::format("{}:{}:fall", names_[val.label_], val.track_id_));
            }else{
                det::draw_bboxes_custom(mat, val, std::format("{}:{}", names_[val.label_], val.track_id_));
            }
        }

        return det_results;
    }


    config_fall_down_obj_det_worker config_;    
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

}

void fall_down_obj_det_worker::process_results(std::any frame)
{
    auto qimg = std::any_cast<QImage>(frame).convertToFormat(QImage::Format_RGB888);
    auto mat = cv::Mat(qimg.height(), qimg.width(), CV_8UC3, qimg.bits(), qimg.bytesPerLine());

    auto const det_results = impl_->track_obj(mat);
    generic_worker_results results;

    if(impl_->config_.roi_.isValid()){
        impl_->scaled_roi_ = convert_qrectf_to_cv_rect(impl_->config_.roi_, mat.cols, mat.rows);
        if(!impl_->scaled_roi_.empty()){
            flt::cvt::utils::draw_empty_rect(mat, impl_->scaled_roi_);
        }
    }

    //do not move it, since in the future this algo may need to support multi-stream
    results.mat_ = qimg;

    emit send_process_results(std::move(results));//*/
}
