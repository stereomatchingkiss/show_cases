#include "paddle_ocr_worker.hpp"

#include "../config/config_paddle_ocr_worker.hpp"

#include "paddle_ocr_worker_results.hpp"

#include <cv_algo/ocr/paddle_ocr/paddle_ocr_det_opencv.hpp>
#include <cv_algo/ocr/paddle_ocr/paddle_ocr_rec_onnx.hpp>

#include <utils/qimage_to_cvmat.hpp>

#include <QDebug>

using namespace flt::mm;
using namespace flt::cvt::ocr;

struct paddle_ocr_worker::impl
{
    impl(config_paddle_ocr_worker const &params) :
        params_{std::move(params)}
        ,text_det_((root_path_ + "ch_PP-OCRv4_det_simple.onnx"))
#ifndef WASM_BUILD
        ,text_rec_((root_path_ + "ch_PP-OCRv4_rec_infer.onnx"),
                   (root_path_ + "paddleocr_keys.txt"))
#endif
    {                
    }

    config_paddle_ocr_worker params_;    

#ifdef WASM_BUILD
    std::string root_path_ = "";
#else
    std::string root_path_ = "assets/";
#endif

    paddle_ocr_det_opencv text_det_;
#ifndef WASM_BUILD
    paddle_ocr_rec_onnx text_rec_;
#endif    
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

    paddle_ocr_worker_results results;
    results.text_boxes_ = impl_->text_det_.predict(mat);

#ifndef WASM_BUILD
    impl_->text_rec_.predict(mat, results.text_boxes_);
    beautify_text_boxes(results.text_boxes_);    
#else
    results.cv_mat_ = mat;
#endif

    if(qimg.format() == QImage::Format_Indexed8 || qimg.format() == QImage::Format_Grayscale8){
        qimg = qimg.convertToFormat(QImage::Format_ARGB32);
    }
    results.mat_ = std::move(qimg);

    emit send_process_results(std::move(results));
}
