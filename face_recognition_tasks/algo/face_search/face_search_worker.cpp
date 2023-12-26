#include "face_search_worker.hpp"

#include "../../global/global_keywords.hpp"

#include "face_search_worker_results.hpp"

#include <cv_algo/converter/qt_and_cv_rect_converter.hpp>
#include <cv_algo/face/face_utils.hpp>
#include <cv_algo/face/det/face_detector_retina_face_ncnn.hpp>
#include <cv_algo/face/reg/face_recognition_arcface_ncnn.hpp>
#include <cv_algo/face/reg/face_warp.hpp>
#include <cv_algo/utils/similarity_compare.hpp>

#include <utils/image_utils.hpp>
#include <utils/qimage_to_cvmat.hpp>

#include <opencv2/imgproc.hpp>

#include <QDebug>

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

#include <QPainter>

using namespace flt::cvt::face;
using namespace flt::cvt::utils;
using namespace flt::mm;

struct face_search_worker::impl
{
    impl() :
        fdet_(global_keywords().retina_face_param(),
              global_keywords().retina_face_bin(), 0.4f),
        freg_(global_keywords().moble_face_v2_param(),
              global_keywords().moble_face_v2_bin(), 112)
    {
        pen_.setColor(Qt::red);
        pen_.setWidth(5);
    }

    std::vector<float> qjson_array_to_float_array(QJsonArray const &arr)
    {
        std::vector<float> results(arr.size());
        for(size_t i = 0; i != arr.size(); ++i){
            results[i] = arr[i].toDouble();
        }

        return results;
    }

    std::tuple<QString, float> closest_features(std::vector<float> const &features)
    {
        float best_score = -100000;
        QString best_name;
        for(auto const &val : features_){
            auto const temp_score = cosine_distance_zscore(val.second, features);
            if(temp_score > best_score){
                best_score = temp_score;
                best_name = val.first;
            }
        }

        return {std::move(best_name), best_score};
    }

    face_search_worker_results register_new_face(QJsonObject const &obj)
    {
        face_search_worker_results results;
        global_keywords const gk;
        auto const fname = obj[gk.face_name()].toString();
        if(auto it = features_.find(fname); it == std::end(features_)){
            features_.emplace(fname, qjson_array_to_float_array(obj[gk.face_features()].toArray()));
            results.obj[gk.job_type()] = "register";
            results.obj[gk.face_name()] = fname;
        }

        return results;
    }

    face_search_worker_results search_best_face(QJsonObject const &obj)
    {
        QImage qimg;
        global_keywords const gk;
        qimg.loadFromData(QByteArray::fromBase64(obj[gk.img()].toString().toLatin1()));

        face_search_worker_results results;
        if(!qimg.isNull()){
            auto cv_mat = std::get<0>(flt::qimg_convert_to_cvmat_non_copy(qimg));

            cv::Mat resize_img;
            cv::resize(cv_mat, resize_img, cv::Size(300, 300));
            auto fdet_resutls = fdet_.predict(resize_img);

            float const x_ratio = cv_mat.cols / 300.0f;
            float const y_ratio = cv_mat.rows / 300.0f;
            QPainter painter(&qimg);
            painter.setPen(pen_);

            QJsonArray info;
            for(size_t i = 0; i != fdet_resutls.size(); ++i){
                for(auto &val : fdet_resutls[i].landmark_pts_){
                    val.x = std::clamp(val.x * x_ratio, 0.f, static_cast<float>(cv_mat.cols - 1));
                    val.y = std::clamp(val.y * y_ratio, 0.f, static_cast<float>(cv_mat.rows - 1));
                }
                auto const face = fwarp_.process(cv_mat, fdet_resutls[i].landmark_pts_);
                auto const face_features = freg_.predict(face);
                auto const rect = fdet_resutls[i].rect_;
                painter.drawRect(rect.x * x_ratio, rect.y * y_ratio, rect.width * x_ratio, rect.height * y_ratio);
                auto [best_name, best_score] = closest_features(face_features);
                painter.drawText(QPoint(rect.x * x_ratio, rect.y * y_ratio),
                                 QString("%1:%2").arg(best_name, QString("%1").arg(best_score, 2)));

                QJsonObject jobj;
                jobj[gk.face_name()] = best_name;
                jobj[gk.face_features()] = best_score;
                info.push_back(jobj);
            }

            results.obj[gk.faces_info()] = info;
            results.img = qimg;
        }

        return results;
    }

    face_search_worker_results process_results(std::any frame)
    {
        QJsonObject obj = QJsonDocument::fromJson(std::any_cast<QString>(frame).toLatin1()).object();
        global_keywords const gk;
        if(obj.contains(gk.job_type())){
            if(auto const dtype = obj[gk.job_type()].toString(); dtype == "register"){
                register_new_face(obj);
            }else if(dtype == "search"){
                return search_best_face(obj);
            }
        }

        return {};
    }

    std::map<QString, std::vector<float>> features_;

    face_detector_retina_face_ncnn fdet_;
    face_recognition_arcface_ncnn freg_;
    face_warp fwarp_;

    QPen pen_;
};

face_search_worker::face_search_worker() :
    frame_process_base_worker(5),
    impl_(std::make_unique<impl>())
{

}

face_search_worker::~face_search_worker()
{

}

void face_search_worker::process_results(std::any frame)
{
    emit send_process_results(impl_->process_results(frame));
}
