#include "nanodet_worker.hpp"

#include "obj_det_worker_results.hpp"

#include "../../config/config_nanodet_worker.hpp"

#include "../../global/global_keywords.hpp"

#include <cv_algo/converter/box_type_converter.hpp>
#include <cv_algo/converter/qt_and_cv_rect_converter.hpp>

#include <cv_algo/obj_detect/obj_det_utils.hpp>
#include <cv_algo/obj_detect/nanodet/nanodet.hpp>
#include <cv_algo/obj_detect/yolo_v8/yolo_v8.hpp>

#include <cv_algo/tracker/track_object_pass.hpp>
#include <cv_algo/tracker/track_results.hpp>
#include <cv_algo/tracker/byte_track/BYTETracker.hpp>

#include <cv_algo/utils/draw_rect.hpp>

#include <utils/file_reader.hpp>

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>

#include <QDebug>
#include <QPixmap>

#include <format>
#include <fstream>

using namespace flt;
using namespace flt::cvt;
using namespace flt::cvt::tracker;

struct nanodet_worker::impl
{
    impl(config_nanodet_worker config) :
        config_{std::move(config)},
        names_{global_keywords().coco_names()}
    {
        using dme = object_detect_model_enum;

        std::string const model_root("assets/obj_detect/");
        switch(config.config_object_detect_model_select_.model_){
        case dme::nanodet_plus_m_320:{
            auto const param = std::format("{}/nanodet-plus-m_{}.param", model_root, 320);
            auto const bin = std::format("{}/nanodet-plus-m_{}.bin", model_root, 320);
            net_ = std::make_unique<cvt::det::nanodet>(param.c_str(), bin.c_str(), 80, false, 320);
            break;
        }
        case dme::nanodet_plus_m_416:{
            auto const param = std::format("{}/nanodet-plus-m_{}.param", model_root, 416);
            auto const bin = std::format("{}/nanodet-plus-m_{}.bin", model_root, 416);
            net_ = std::make_unique<cvt::det::nanodet>(param.c_str(), bin.c_str(), 80, false, 416);
            break;
        }
        case dme::yolo_v8_n:{
            auto const param = std::format("{}/yolov8n.param", model_root);
            auto const bin = std::format("{}/yolov8n.bin", model_root);
            net_ = std::make_unique<cvt::det::yolo_v8>(param.c_str(), bin.c_str(), 80, false, 416);
            break;
        }
        }

        create_obj_to_detect();        
    }

    void create_obj_to_detect()
    {
        obj_to_detect_.resize(80);
        auto const &so = config_.config_select_object_to_detect_.selected_object_;
        for(size_t i = 0; i != names_.size(); ++i){
            if(so.contains(names_[i])){
                obj_to_detect_[i] = true;
            }
        }
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

    void remove_invalid_target(std::vector<det::box_info> &det_results)
    {
        auto func = [this](det::box_info const &val){
            return !obj_to_detect_[val.label_];
        };
        const auto [first, last] = std::ranges::remove_if(det_results, func);
        det_results.erase(first, last);
    }

    auto track_obj(cv::Mat &mat)
    {
        auto det_results = net_->predict(mat, config_.config_object_detect_model_select_.confidence_,
                                         config_.config_object_detect_model_select_.nms_);
        remove_invalid_target(det_results);
        auto track_obj = box_info_to_byte_track_obj(det_results);
        auto const track_ptr_vec = tracker_.update(track_obj);

        det_results = byte_track_obj_to_box_info(track_ptr_vec);
        for(auto const &val : det_results){
            det::draw_bboxes_custom(mat, val, std::format("{}:{}", names_[val.label_], val.track_id_));
        }

        return det_results;
    }

    config_nanodet_worker config_;
    std::vector<std::string> names_;    
    std::unique_ptr<det::obj_det_base> net_;
    std::vector<bool> obj_to_detect_;
    cv::Rect scaled_roi_;
    BYTETracker tracker_;
    std::unique_ptr<track_object_pass> track_obj_pass_;
};

nanodet_worker::nanodet_worker(config_nanodet_worker config, QObject *parent) :
    flt::mm::frame_process_base_worker(parent),
    impl_{std::make_unique<impl>(std::move(config))}
{

}

nanodet_worker::~nanodet_worker()
{

}

void nanodet_worker::process_results(std::any frame)
{
    auto mat = std::any_cast<cv::Mat>(frame);
    if(mat.channels() == 3){
        cv::cvtColor(mat, mat, cv::COLOR_BGR2RGB);
    }else{
        cv::cvtColor(mat, mat, cv::COLOR_GRAY2RGB);
    }

    if(!impl_->track_obj_pass_){
        impl_->scaled_roi_ = convert_qrectf_to_cv_rect(impl_->config_.roi_, mat.cols, mat.rows);
        impl_->track_obj_pass_ =
            std::make_unique<cvt::tracker::track_object_pass>(impl_->scaled_roi_, 30);
    }

    auto const det_results = impl_->track_obj(mat);
    auto const pass_results = impl_->track_obj_pass_->track(det_results);    
    impl_->draw_pass_results(mat, pass_results);

    obj_det_worker_results results;
    if(impl_->config_.config_tracker_alert_.alert_if_stay_in_roi_on_){
        for(auto const &val : pass_results.track_durations_){
            if(val.duration_sec_ >= impl_->config_.config_tracker_alert_.alert_if_stay_in_roi_duration_sec_){
                results.alarm_on_ = true;
            }
        }
    }

    results.mat_ = std::move(mat);

    emit send_process_results(results);
}
