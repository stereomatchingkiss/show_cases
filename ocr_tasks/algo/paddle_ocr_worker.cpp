#include "paddle_ocr_worker.hpp"

#include "../config/config_paddle_ocr_worker.hpp"

#include "paddle_ocr_worker_results.hpp"

#include <cv_algo/ocr/paddle_ocr/paddle_ocr_text_detector.hpp>
#include <cv_algo/ocr/paddle_ocr/paddle_ocr_text_rec.hpp>

#include <utils/qimage_to_cvmat.hpp>

#include <QDebug>

using namespace flt::mm;
using namespace flt::cvt::ocr;

struct paddle_ocr_worker::impl
{
    impl(config_paddle_ocr_worker const &params) :
        params_{std::move(params)},
        text_det_((root_path_ + "ch_PP-OCRv3_det.param").c_str(), (root_path_ + "ch_PP-OCRv3_det.bin").c_str()),
        text_rec_((root_path_ + "ch_PP-OCRv3_rec.param").c_str(),
                  (root_path_ + "ch_PP-OCRv3_rec.bin").c_str(),
                  (root_path_ + "paddleocr_keys.txt").c_str())
    {
        qDebug()<<"text_det_ load = "<<text_det_.get_load_model_state();
        qDebug()<<"text_rec_ load = "<<text_rec_.get_load_model_state();        
    }

    config_paddle_ocr_worker params_;    

#ifdef WASM_BUILD
    std::string root_path_ = "";
#else
    std::string root_path_ = "assets/";
#endif

    paddle_ocr_text_detector text_det_;
    paddle_ocr_text_rec text_rec_;
};

paddle_ocr_worker::paddle_ocr_worker(config_paddle_ocr_worker const &params, QObject *parent) :
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
    auto mat = std::get<0>(flt::qimg_convert_to_cvmat_non_copy(qimg));

    if(mat.empty()){
        qDebug()<<"cannot convert qimg with format = "<<qimg.format();
        return;
    }

    std::cout<<"mat is continuous = "<<mat.isContinuous();
    if(!mat.isContinuous()){
        mat = mat.clone();
    }

    for(size_t i = 0; i != 10; ++i){
        std::cout<<i<<":"<<mat.at<cv::Vec3b>(i)<<std::endl;
    }
    for(size_t i = mat.rows; i != mat.rows + 10; ++i){
        std::cout<<i<<":"<<mat.at<cv::Vec3b>(i)<<std::endl;
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

    results.mat_ = std::move(qimg);

    emit send_process_results(std::move(results));
}
