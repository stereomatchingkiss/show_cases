#include "anpr_worker.hpp"

#include "anpr_algo.hpp"

#include "../config/config_anpr_worker.hpp"

#include <cv_algo/converter/qt_and_cv_rect_converter.hpp>

#include <QDebug>

#include <QPainter>
#include <QPen>

using namespace flt::cvt;

struct anpr_worker::impl
{
    impl(config_anpr_worker const &config) :
        algo_{config.config_object_detect_model_select_}
    {
        pen_plate_.setColor(Qt::darkRed);
        pen_text_.setColor(Qt::yellow);
        pen_text_.setWidth(10);
        pen_vechile_.setColor(Qt::green);
    }

    void predict(QImage &img)
    {
        auto const pre_results = algo_.predict(img);        
        QPainter painter(&img);
        for(auto const &val : pre_results){
            painter.setPen(pen_vechile_);
            painter.drawRect(convert_cv_rect_to_qrect(val.vehicle_rect_));
            if(!val.plate_rect_.empty()){
                painter.setPen(pen_plate_);
                painter.drawRect(convert_cv_rect_to_qrect(val.plate_rect_));
                painter.setPen(pen_text_);
                painter.drawText(QPoint(val.plate_rect_.x, val.plate_rect_.y - 5), val.plate_num_);
            }
        }
    }

    anpr_algo algo_;
    config_anpr_worker config_;
    QPen pen_plate_;
    QPen pen_text_;
    QPen pen_vechile_;
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
    impl_->predict(qimg);

    emit send_process_results(qimg);
}
