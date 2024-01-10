#include "anpr_worker.hpp"

#include "anpr_algo.hpp"

#include "anpr_worker_results.hpp"

#include "../config/config_anpr_worker.hpp"

#include <QDebug>

#include <QPainter>
#include <QPen>

struct anpr_worker::impl
{
    impl(config_anpr_worker const &config) :
        algo_{config.config_object_detect_model_select_}
    {        
    }

    anpr_worker_results predict(QImage &img)
    {
        anpr_worker_results results;
        results.full_results_ = algo_.predict(img);
        results.qimg_ = img;

        for(auto const &val : results.full_results_){
            if(!val.plate_num_.isEmpty() && !val.plate_rect_.empty()){
                results.plate_results_.emplace_back(val);
            }
        }

        for(auto &val : results.plate_results_){
            val.vehicle_rect_ = cv::Rect();
        }

        return results;
    }

    anpr_algo algo_;
    config_anpr_worker config_;    
};

anpr_worker::anpr_worker(config_anpr_worker const &config, QObject *parent) :
    flt::mm::frame_process_base_worker(2, parent),
    impl_{std::make_unique<impl>(config)}
{

}

anpr_worker::~anpr_worker()
{

}

void anpr_worker::process_results(std::any frame)
{    
    auto qimg = std::any_cast<QImage>(frame);    

    emit send_process_results(impl_->predict(qimg));
}
