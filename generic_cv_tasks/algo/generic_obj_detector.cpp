#include "generic_obj_detector.hpp"

#include "../config/config_generic_obj_detector.hpp"

#include "../global/global_keywords.hpp"

#include <cv_algo/obj_detect/nanodet/nanodet.hpp>
#include <cv_algo/obj_detect/yolo_v8/yolo_v8.hpp>

#include <QDebug>

#include <format>
#include <memory>

using namespace flt;
using namespace flt::cvt;
using namespace flt::cvt::det;

struct generic_obj_detector::impl
{
    impl(config_generic_obj_detector config) :
        config_{std::move(config)},
        names_{global_keywords().coco_names()}
    {
        create_model();
        create_obj_to_detect();
    }

    void create_model()
    {
        using dme = object_detect_model_enum;

#ifndef WASM_BUILD
        std::string const model_root("assets/obj_detect/");
#else
        std::string const model_root("");
#endif
        switch(config_.config_object_detect_model_select_.model_){
        case dme::nanodet_plus_m_320:{
            qDebug()<<"load nanodet_plus_m_320";
            auto const param = std::format("{}nanodet-plus-m_{}.param", model_root, 320);
            auto const bin = std::format("{}nanodet-plus-m_{}.bin", model_root, 320);
            net_ = std::make_unique<cvt::det::nanodet>(param.c_str(), bin.c_str(), 80, false, 320);
            break;
        }
        case dme::nanodet_plus_m_416:{
            qDebug()<<"load nanodet_plus_m_416";
            auto const param = std::format("{}nanodet-plus-m_{}.param", model_root, 416);
            auto const bin = std::format("{}nanodet-plus-m_{}.bin", model_root, 416);
            net_ = std::make_unique<cvt::det::nanodet>(param.c_str(), bin.c_str(), 80, false, 416);
            break;
        }
        case dme::nanodet_plus_m_1_5x_320:{
            qDebug()<<"load nanodet_plus_m_1_5x_320";
            auto const param = std::format("{}nanodet-plus-m-1.5x_{}_opt.param", model_root, 320);
            auto const bin = std::format("{}nanodet-plus-m-1.5x_{}_opt.bin", model_root, 320);
            net_ = std::make_unique<cvt::det::nanodet>(param.c_str(), bin.c_str(), 80, false, 320);
            break;
        }
        case dme::nanodet_plus_m_1_5x_416:{
            qDebug()<<"load nanodet_plus_m_1_5x_416";
            auto const param = std::format("{}nanodet-plus-m-1.5x_{}_opt.param", model_root, 416);
            auto const bin = std::format("{}nanodet-plus-m-1.5x_{}_opt.bin", model_root, 416);
            net_ = std::make_unique<cvt::det::nanodet>(param.c_str(), bin.c_str(), 80, false, 416);
            break;
        }
        case dme::yolo_v8_n_416:{
            qDebug()<<"load yolo_v8_n_416";
            auto const param = std::format("{}yolov8n.param", model_root);
            auto const bin = std::format("{}yolov8n.bin", model_root);
            net_ = std::make_unique<cvt::det::yolo_v8>(param.c_str(), bin.c_str(), 80, false, 416);
            break;
        }
        }
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

    std::vector<box_info> predict(cv::Mat &rgb)
    {
        auto det_results = net_->predict(rgb,
                                         config_.config_object_detect_model_select_.confidence_,
                                         config_.config_object_detect_model_select_.nms_,
                                         0,
                                         false);
        remove_invalid_target(det_results);

        return det_results;
    }

    void remove_invalid_target(std::vector<det::box_info> &det_results)
    {
        auto func = [this](det::box_info const &val){
            return !obj_to_detect_[val.label_];
        };
        const auto [first, last] = std::ranges::remove_if(det_results, func);
        det_results.erase(first, last);
    }

    config_generic_obj_detector config_;
    std::vector<std::string> names_;
    std::unique_ptr<det::obj_det_base> net_;
    std::vector<bool> obj_to_detect_;
};

generic_obj_detector::generic_obj_detector(config_generic_obj_detector config) :
    impl_{std::make_unique<impl>(config)}
{

}

generic_obj_detector::~generic_obj_detector()
{

}

std::vector<box_info> generic_obj_detector::predict(cv::Mat &rgb)
{
    return impl_->predict(rgb);
}
