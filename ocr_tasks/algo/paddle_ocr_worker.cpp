#include "paddle_ocr_worker.hpp"

#include "paddle_ocr_det_opencv.hpp"

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
        ocv_text_det_{root_path_ + "ch_PP-OCRv3_det_simple.onnx"},
        text_det_((root_path_ + "pdocrv2.0_det-op.param").c_str(), (root_path_ + "pdocrv2.0_det-op.bin").c_str()),
        text_rec_((root_path_ + "pdocrv2.0_rec-op.param").c_str(),
                  (root_path_ + "pdocrv2.0_rec-op.bin").c_str(),
                  (root_path_ + "paddleocr_keys.txt").c_str(),
                  32)
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

    paddle_ocr_det_opencv ocv_text_det_;

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

    std::cout<<"mat is continuous = "<<mat.isContinuous()<<std::endl;
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
    for(auto const &val : results.text_boxes_){
        for(auto const &pt : val.boxPoint){
            std::cout<<pt<<",";
        }
        std::cout<<std::endl;
    }//*/

    /*std::vector<std::vector<cv::Point>> points{{{168, 216},{498, 224},{497, 274},{167, 266}},
                                               {{82, 278},{587, 278},{587, 331},{82, 331}},
                                               {{154, 336},{513, 346},{512, 399},{153, 388}},
                                               {{80, 396},{589, 399},{588, 458},{79, 455}}};
    for(size_t i = 0; i != 4; ++i){
        TextBox box;
        box.boxPoint = std::move(points[i]);
        results.text_boxes_.push_back(box);
    }

    for(auto const &val : results.text_boxes_){
        for(auto const &pt : val.boxPoint){
            std::cout<<pt<<",";
        }
        std::cout<<std::endl;
    }//*/
    //[168, 216],[498, 224],[497, 274],[167, 266],
    //[82, 278],[587, 278],[587, 331],[82, 331],
    //[154, 336],[513, 346],[512, 399],[153, 388],
    //[80, 396],[589, 399],[588, 458],[79, 455],
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

    results.mat_ = std::move(qimg);//*/

    /*paddle_ocr_worker_results results;
    auto output = impl_->ocv_text_det_.predict(mat);
    QImage img(output.data, output.cols, output.rows, output.step, QImage::Format_Indexed8);
    results.mat_ = img.convertToFormat(QImage::Format_ARGB32);//*/

    emit send_process_results(std::move(results));
}
