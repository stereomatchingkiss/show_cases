#include "nanodet_worker.hpp"

#include <cv_algo/obj_detect/nanodet/nanodet.hpp>
#include <cv_algo/obj_detect/nanodet/nanodet_utils.hpp>

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>

#include <QPixmap>

#include <fstream>

struct nanodet_worker::impl
{
    impl()
        : net_("assets/obj_detect/nanodet-plus-m_320_opt.param",
               "assets/obj_detect/nanodet-plus-m_320_opt.bin", 80, false)
    {
        std::ifstream in("assets/obj_detect/coco.names");
        std::string line;
        while(std::getline(in, line)){
            names_.emplace_back(std::move(line));
        }
    }

    ocv::nanodet net_;
    std::vector<std::string> names_;
};

nanodet_worker::nanodet_worker(QObject *parent) :
    ocv::frame_process_base_worker(parent),
    impl_{std::make_unique<impl>()}
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

    auto const det_results = impl_->net_.predict(mat, 0.6f, 0.3f);
    ocv::draw_bboxes(mat, det_results, impl_->names_);

    auto qimg = QImage(mat.data, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);
    emit send_process_results(QPixmap::fromImage(std::move(qimg)));
}
