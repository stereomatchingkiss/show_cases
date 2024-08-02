#include "generic_obj_detector.hpp"

#include "../config/config_generic_obj_detector.hpp"

#include "../global/global_keywords.hpp"

#include <cv_algo/obj_detect/nanodet/nanodet.hpp>
#include <cv_algo/obj_detect/yolo_v8/yolo_v8.hpp>
#include <utils/meyers_singleton.hpp>

#include <QDebug>

#include <atomic>
#include <format>
#include <memory>

using namespace flt;
using namespace flt::cvt;
using namespace flt::cvt::det;

struct generic_obj_detector::impl
{
    impl(config_generic_obj_detector config) :
        config_{std::move(config)},
        names_{global_keywords().coco_names()},
        net_{nullptr}
    {
        nets_.resize(10);

        create_model();
        create_obj_to_detect();
    }

    ~impl()
    {
        nets_[model_index_].delete_net();
    }

    template<typename T = cvt::det::nanodet>
    requires std::is_class_v<T>
    void create_model(std::string const &param,
                      std::string const &bin,
                      int num_class,
                      bool use_gpu = false,
                      int input_size = 320,
                      size_t model_index = 0)
    {
        model_index_ = use_gpu ? model_index : model_index + 1;
        net_ = nets_[model_index_].create_net<T>(param.c_str(), bin.c_str(), num_class, use_gpu, input_size);
    }

    void create_model()
    {
        using dme = object_detect_model_enum;

#ifndef WASM_BUILD
        std::string const model_root("assets/obj_detect/");
#else
        std::string const model_root("");
#endif
        auto const use_gpu = config_.config_object_detect_model_select_.use_gpu_;
        switch(config_.config_object_detect_model_select_.model_){
        case dme::nanodet_plus_m_320:{
            qDebug()<<"load nanodet_plus_m_320";
            auto const param = std::format("{}nanodet-plus-m_{}.param", model_root, 320);
            auto const bin = std::format("{}nanodet-plus-m_{}.bin", model_root, 320);            
            create_model(param, bin, 80, use_gpu, 320, 0);
            break;
        }
        case dme::nanodet_plus_m_416:{
            qDebug()<<"load nanodet_plus_m_416";
            auto const param = std::format("{}nanodet-plus-m_{}.param", model_root, 416);
            auto const bin = std::format("{}nanodet-plus-m_{}.bin", model_root, 416);            
            create_model(param, bin, 80, use_gpu, 416, 2);
            break;
        }
        case dme::nanodet_plus_m_1_5x_320:{
            qDebug()<<"load nanodet_plus_m_1_5x_320";
            auto const param = std::format("{}nanodet-plus-m-1.5x_{}_opt.param", model_root, 320);
            auto const bin = std::format("{}nanodet-plus-m-1.5x_{}_opt.bin", model_root, 320);            
            create_model(param, bin, 80, use_gpu, 320, 4);
            break;
        }
        case dme::nanodet_plus_m_1_5x_416:{
            qDebug()<<"load nanodet_plus_m_1_5x_416";
            auto const param = std::format("{}nanodet-plus-m-1.5x_{}_opt.param", model_root, 416);
            auto const bin = std::format("{}nanodet-plus-m-1.5x_{}_opt.bin", model_root, 416);            
            create_model(param, bin, 80, use_gpu, 416, 6);
            break;
        }
        case dme::yolo_v8_n_416:{
            qDebug()<<"load yolo_v8_n_416";
            auto const param = std::format("{}yolov8n.param", model_root);
            auto const bin = std::format("{}yolov8n.bin", model_root);
            create_model<cvt::det::yolo_v8>(param, bin, 80, use_gpu, 416, 8);
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

    struct obj_det_struct
    {
        obj_det_struct()
        {
            std::lock_guard<std::mutex> lock(mutex_);
            count_ = 0;
        }

        obj_det_struct(obj_det_struct const &val)
        {
            std::lock_guard<std::mutex> lock(mutex_);
            count_ = val.count_;
        }

        ~obj_det_struct(){}

        template<typename T>
        requires std::is_class_v<T>
        det::obj_det_base* create_net(const char* param,
                                      const char* bin,
                                      int num_class,
                                      bool use_gpu = false,
                                      int input_size = 320)
        {
            std::lock_guard<std::mutex> lock(mutex_);
            if(!net_){
                net_ = std::make_unique<T>(param, bin, num_class, use_gpu, input_size);
            }

            ++count_;

            return net_.get();
        }

        void delete_net()
        {
            if(count_ > 0){
                --count_;
            }

            if(count_ == 0){
                net_.reset();
            }
        }

        size_t count_;
        std::mutex mutex_;
        std::unique_ptr<det::obj_det_base> net_;
    };

    static std::vector<obj_det_struct> nets_;
    config_generic_obj_detector config_;
    std::vector<std::string> names_;
    size_t model_index_ = 0;
    det::obj_det_base *net_;
    std::vector<bool> obj_to_detect_;
};

std::vector<generic_obj_detector::impl::obj_det_struct> generic_obj_detector::impl::nets_;

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
