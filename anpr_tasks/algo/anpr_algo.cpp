#include "anpr_algo.hpp"

#include "anpr_algo_predict_results.hpp"

#include "../config/config_object_detect_model_select.hpp"
#include "../global/global_keywords.hpp"

#include <cv_algo/converter/box_type_converter.hpp>
#include <cv_algo/converter/qt_and_cv_rect_converter.hpp>

#include <cv_algo/obj_detect/nanodet/nanodet.hpp>
#include <cv_algo/obj_detect/nanodet/nanodet_utils.hpp>
#include <cv_algo/obj_detect/obj_det_utils.hpp>

#include <cv_algo/ocr/paddle_ocr/paddle_ocr_rec_opencv.hpp>

#include <utils/qimage_to_cvmat.hpp>

#include <QDebug>
#include <QImage>

#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include <iterator>

using namespace flt::cvt::det;
using namespace flt::cvt::ocr;

struct anpr_algo::impl
{
    impl(config_object_detect_model_select const &config_model_select) :
        config_model_det_{config_model_select},
        det_{create_nanodet(config_model_select.model_)},
        ocr_rec_{create_paddle_ocr_rec()}
    {

    }

    std::unique_ptr<nanodet> create_nanodet(object_detect_model_enum mtype) const
    {
        if(mtype == object_detect_model_enum::nanodet_plus_m_320){
            return std::make_unique<nanodet>(global_keywords().nanodet_320_param_path().c_str(),
                                             global_keywords().nanodet_320_bin_path().c_str(),
                                             2, false, 320);
        }

        return std::make_unique<nanodet>(global_keywords().nanodet_320_param_path().c_str(),
                                         global_keywords().nanodet_320_bin_path().c_str(),
                                         2, false, 416);
    }

    std::unique_ptr<paddle_ocr_rec_opencv> create_paddle_ocr_rec() const
    {
        return std::make_unique<paddle_ocr_rec_opencv>(global_keywords().paddle_ocr_rec_model_path(),
                                                       global_keywords().anpr_task_assets() + "/ppocr_keys_v1.txt",
                                                       48, 320);
    }

    template<std::forward_iterator T>
    auto find_plate_of_vehicle(cv::Rect const &car_rect, T plate_beg_it, T plate_end_it) const noexcept
    {
        return std::find_if(plate_beg_it, plate_end_it, [car_rect](box_info const &plate_box)
                            {
                                auto const plate_rect = plate_box.to_cv_rect();
                                return (plate_rect & car_rect) == plate_rect;
                            });
    }

    TextBox ocr_reg(cv::Mat const &bgr, std::vector<cv::Point> pts) const
    {
        TextBox tbox;
        tbox.boxPoint = std::move(pts);
        std::vector<TextBox> tboxes;
        tboxes.emplace_back(std::move(tbox));

        ocr_rec_->predict(bgr, tboxes);

        return std::move(tboxes[0]);
    }

    std::vector<anpr_algo_predict_results> predict(cv::Mat const &bgr)
    {
        auto obj_det_res = det_->predict(bgr, config_model_det_.confidence_, config_model_det_.nms_);
        auto car_end_it = std::partition(std::begin(obj_det_res), std::end(obj_det_res), [](auto const &val)
                                         {
                                             return val.label_ == 0;
                                         });

        std::vector<anpr_algo_predict_results> outputs;
        for(auto car_beg_it = std::begin(obj_det_res); car_beg_it != car_end_it; ++car_beg_it){
            anpr_algo_predict_results result;
            result.vehicle_rect_ = car_beg_it->to_cv_rect();

            //Vehicle is easier to find, this algorithm only perform ocr if it could find the
            //plate within the bounding rect of the vehicle
            if(auto plate_it = find_plate_of_vehicle(car_beg_it->to_cv_rect(), car_end_it, std::end(obj_det_res));
                plate_it != std::end(obj_det_res)){
                TextBox tbox = ocr_reg(bgr, plate_it->box_pts());
                result.plate_num_ = tbox.text.c_str();
                result.plate_rect_ = cv::boundingRect(tbox.boxPoint);
            }

            outputs.emplace_back(std::move(result));
        }

        return outputs;
    }

    std::vector<anpr_algo_predict_results> predict(QImage &rgb)
    {
        if(auto cv_img = std::get<0>(flt::qimg_convert_to_cvmat_non_copy(rgb)); !cv_img.empty()){
            cv::cvtColor(cv_img, bgr_, cv::COLOR_RGB2BGR);
            return predict(bgr_);
        }

        return {};
    }

    config_object_detect_model_select config_model_det_;

    cv::Mat bgr_;
    std::unique_ptr<nanodet> det_;
    std::unique_ptr<paddle_ocr_rec_opencv> ocr_rec_;
};

anpr_algo::anpr_algo(config_object_detect_model_select const &config_model_select) :
    impl_{std::make_unique<impl>(config_model_select)}
{

}

anpr_algo::~anpr_algo()
{

}

std::vector<anpr_algo_predict_results> anpr_algo::predict(QImage &rgb)
{
    return impl_->predict(rgb);
}

std::vector<anpr_algo_predict_results> anpr_algo::predict(const cv::Mat &bgr)
{
    return impl_->predict(bgr);
}
