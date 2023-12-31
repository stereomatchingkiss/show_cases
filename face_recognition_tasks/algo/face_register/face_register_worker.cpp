#include "face_register_worker.hpp"

#include "../../global/global_keywords.hpp"

#include "face_register_results.hpp"

#include <cv_algo/face/face_utils.hpp>
#include <cv_algo/face/det/face_detector_retina_face_ncnn.hpp>
#include <cv_algo/face/reg/face_recognition_arcface_ncnn.hpp>
#include <cv_algo/face/reg/face_warp.hpp>

#include <cv_algo/converter/qt_and_cv_rect_converter.hpp>
#include <utils/qimage_to_cvmat.hpp>

#ifdef WASM_BUILD

#include <utils/image_utils.hpp>

#endif

#include <opencv2/imgproc.hpp>

#include <QDebug>

using namespace flt::cvt::face;
using namespace flt::mm;

struct face_register_worker::impl
{
    impl() :
        fdet_(global_keywords().retina_face_param(),
              global_keywords().retina_face_bin(), 0.4f),
        freg_(global_keywords().moble_face_v2_param(),
              global_keywords().moble_face_v2_bin(), 112)
    {

    }

    cv::Mat to_cv_mat(QImage &qimg) const
    {
        return std::get<0>(flt::qimg_convert_to_cvmat_non_copy(qimg));
    }

    face_register_results predict(cv::Mat const &cv_mat)
    {
        cv::Mat resize_img;
        cv::resize(cv_mat, resize_img, cv::Size(300, 300));
        auto fdet_resutls = fdet_.predict(resize_img);

        face_register_results results;
        float const x_ratio = cv_mat.cols / 300.0f;
        float const y_ratio = cv_mat.rows / 300.0f;
        for(size_t i = 0; i != fdet_resutls.size(); ++i){
            for(auto &val : fdet_resutls[i].landmark_pts_){
                val.x = std::clamp(val.x * x_ratio, 0.f, static_cast<float>(cv_mat.cols - 1));
                val.y = std::clamp(val.y * y_ratio, 0.f, static_cast<float>(cv_mat.rows - 1));
            }
            auto face = fwarp_.process(cv_mat, fdet_resutls[i].landmark_pts_);
            QImage img(face.data, face.cols, face.rows, QImage::Format_RGB888);

            results.faces_.emplace_back(img.rgbSwapped());
            results.features_.emplace_back(freg_.predict(face));
        }

        return results;
    }

    face_detector_retina_face_ncnn fdet_;
    face_recognition_arcface_ncnn freg_;
    face_warp fwarp_;
};

face_register_worker::face_register_worker() :
    frame_process_base_worker(5),
    impl_{std::make_unique<impl>()}
{

}

face_register_worker::~face_register_worker()
{

}

void face_register_worker::process_results(std::any frame)
{
    auto qimg = std::any_cast<QImage>(frame);
    auto cv_mat = impl_->to_cv_mat(qimg);
    auto pre_results = impl_->predict(cv_mat);

    emit send_process_results(pre_results);
}
