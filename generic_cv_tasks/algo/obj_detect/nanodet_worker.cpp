#include "nanodet_worker.hpp"

#include <cv_algo/converter/box_type_converter.hpp>
#include <cv_algo/obj_detect/nanodet/nanodet.hpp>
#include <cv_algo/obj_detect/nanodet/nanodet_utils.hpp>
#include <cv_algo/tracker/byte_track/BYTETracker.hpp>
#include <cv_algo/utils/draw_rect.hpp>
#include <utils/file_reader.hpp>

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>

#include <QDebug>
#include <QPixmap>

#include <format>
#include <fstream>

struct nanodet_worker::impl
{
    impl(QRectF const &rband,
         float score_threshold,
         float nms_threshold,
         int input_size)
        : names_{ocv::read_file_per_lines("assets/obj_detect/coco.names")},
        net_(std::format("assets/obj_detect/nanodet-plus-m_{}.param", input_size).c_str(),
             std::format("assets/obj_detect/nanodet-plus-m_{}.bin", input_size).c_str(), 80, false, false, input_size),
        input_size_{input_size},
        nms_threshold_{nms_threshold},
        rband_{rband},
        score_threshold_{score_threshold}
    {
    }

    std::vector<std::string> names_;
    ocv::det::nanodet net_;
    ocv::byte_track::BYTETracker tracker_;

    int input_size_;
    float nms_threshold_;
    QRectF rband_;
    float score_threshold_;
};

nanodet_worker::nanodet_worker(QRectF const &rband,
                               float score_threshold,
                               float nms_threshold,
                               int input_size,
                               QObject *parent) :
    ocv::frame_process_base_worker(parent),
    impl_{std::make_unique<impl>(rband, score_threshold, nms_threshold, input_size)}
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

    auto det_results = impl_->net_.predict(mat, impl_->score_threshold_, impl_->nms_threshold_);
    const auto [first, last] = std::ranges::remove_if(det_results, [](auto const &val)
                                                      {
                                                          return val.label_ != 2;
                                                      });
    det_results.erase(first, last);
    auto track_obj = ocv::box_info_to_byte_track_obj(det_results);
    auto const track_ptr_vec = impl_->tracker_.update(track_obj);

    det_results = ocv::byte_track_obj_to_box_info(track_ptr_vec, 2);
    for(auto const &val : det_results){
        ocv::det::draw_bboxes_custom(mat, val, std::format("{}:{}", impl_->names_[2], val.track_id_));
    }

    if(!impl_->rband_.isEmpty()){
        ocv::utils::draw_empty_rect(mat, impl_->rband_);
    }

    auto qimg = QImage(mat.data, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);
    emit send_process_results(QPixmap::fromImage(std::move(qimg)));
}
