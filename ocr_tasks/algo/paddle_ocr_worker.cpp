#include "paddle_ocr_worker.hpp"

#include "../config/config_paddle_ocr_worker.hpp"

#include "paddle_ocr_worker_results.hpp"

#include <cv_algo/ocr/paddle_ocr/paddle_ocr_text_detector.hpp>
#include <cv_algo/ocr/paddle_ocr/paddle_ocr_text_rec.hpp>

#include <utils/qimage_to_cvmat.hpp>

#include <QDebug>
#include <QPainter>

using namespace flt::mm;
using namespace flt::cvt::ocr;

struct paddle_ocr_worker::impl
{
    impl(config_paddle_ocr_worker params) :
        params_{std::move(params)},
        text_det_((root_path_ + "ch_PP-OCRv4_det_opt.param").c_str(), (root_path_ + "ch_PP-OCRv4_det_opt.bin").c_str()),
        text_rec_((root_path_ + "ch_PP-OCRv3_rec.param").c_str(),
                  (root_path_ + "ch_PP-OCRv3_rec.bin").c_str(),
                  (root_path_ + "paddleocr_keys.txt").c_str())
    {
        qDebug()<<"text_det_ load = "<<text_det_.get_load_model_state();
        qDebug()<<"text_rec_ load = "<<text_rec_.get_load_model_state();

        pen_.setColor(Qt::green);
        pen_.setWidth(5);
    }

    config_paddle_ocr_worker params_;
    QPen pen_;

#ifdef WASM_BUILD
    std::string root_path_ = "";
#else
    std::string root_path_ = "assets/";
#endif

    paddle_ocr_text_detector text_det_;
    paddle_ocr_text_rec text_rec_;
};

paddle_ocr_worker::paddle_ocr_worker(config_paddle_ocr_worker params, QObject *parent) :
    frame_process_base_worker(2, parent),
    impl_(std::make_unique<impl>(params))
{

}

paddle_ocr_worker::~paddle_ocr_worker()
{

}

void paddle_ocr_worker::process_results(std::any frame)
{    
    auto qimg = std::any_cast<QImage>(frame);
    auto const mat = std::get<0>(flt::qimg_convert_to_cvmat_non_copy(qimg));

    if(mat.empty()){
        qDebug()<<"cannot convert qimg with format = "<<qimg.format();
        return;
    }

    paddle_ocr_worker_results results;
    results.text_boxes_ = impl_->text_det_.predict(mat);
    impl_->text_rec_.predict(mat, results.text_boxes_);
    auto [it_start, it_end] = std::ranges::remove_if(results.text_boxes_, [](auto const &val)
                                                     {
                                                         return val.text.empty();
                                                     });
    results.text_boxes_.erase(it_start, it_end);

    std::ranges::sort(results.text_boxes_, [](TextBox const &a, TextBox const &b)
                      {
                          return std::tie(a.boxPoint[0].y, a.boxPoint[0].x) < std::tie(b.boxPoint[0].y, b.boxPoint[0].x);
                      });

    QPainter painter(&qimg);
    painter.setPen(impl_->pen_);
    for(auto const &val : results.text_boxes_){
        QPolygon poly;
        for(auto const &pt : val.boxPoint){
            poly<<QPoint(pt.x, pt.y);
        }
        painter.drawPolygon(poly);
    }

    results.mat_ = std::move(qimg);

    emit send_process_results(std::move(results));
}
