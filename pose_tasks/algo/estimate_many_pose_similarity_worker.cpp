#include "estimate_many_pose_similarity_worker.hpp"

#include "estimate_many_pose_similarity_worker_results.hpp"
#include "../algo/pose_estimation_common_func.hpp"
#include "../config/config_estimate_many_pose_similarity_worker.hpp"

#include <cv_algo/pose/movenet_single_pose_estimate.hpp>
#include <cv_algo/pose/pose_estimation_utils.hpp>
#include <cv_algo/pose/pose_similarity_estimation.hpp>
#include <cv_algo/pose/pose_similarity_search.hpp>
#include <cv_algo/pose/pose_similarity_search_results.hpp>
#include <utils/qimage_to_cvmat.hpp>

#include <QDebug>

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

#include <opencv2/imgproc.hpp>

using namespace flt::cvt::pose;

struct estimate_many_pose_similarity_worker::impl
{
    impl(config_estimate_many_pose_similarity_worker const &config) :
        config_{config},
        net_{pose_param_path(), pose_bin_path(), 256, false},
        search_{10}
    {

    }

    estimate_many_pose_similarity_worker_results add(QJsonObject const &jobj, QString const &mode)
    {
        if(mode == "add" && jobj.contains("im")){
            QImage qimg;
            qimg.loadFromData(QByteArray::fromBase64(jobj["im"].toString().toLatin1()), "JPG");
            if(!qimg.isNull()){
                auto results = predict_pose<estimate_many_pose_similarity_worker_results>(qimg, config_.confidence_, net_);
                results.im_path_ = jobj["im_path"].toString();
                qDebug()<<jobj["im_path"].toString();
                search_.add_pose(jobj["im_path"].toString().toStdString(), results.points_);

                return results;
            }
        }

        return {};
    }

    estimate_many_pose_similarity_worker_results add_by_json(QJsonObject const &jobj, QString const&)
    {
        estimate_many_pose_similarity_worker_results results;
        results.im_path_ = jobj["im_path"].toString();
        auto const pts_arr = jobj["pts"].toArray();
        for(int i = 0; i != pts_arr.size(); ++i){
            keypoint kpt;
            auto const obj = pts_arr[i].toObject();
            kpt.x_ = obj["x"].toInt();
            kpt.y_ = obj["y"].toInt();
            kpt.hx_ = obj["hx"].toInt();
            kpt.hy_ = obj["hy"].toInt();
            kpt.score_ = obj["score"].toDouble();
            results.points_.emplace_back(kpt);
        }

        results.qimg_.loadFromData(QByteArray::fromBase64(jobj["im"].toString().toLatin1()), "JPG");
        qDebug()<<jobj["im_path"].toString();
        search_.add_pose(jobj["im_path"].toString().toStdString(), results.points_);

        return results;
    }

    std::tuple<pose_similarity_search_results, QImage> find_similar_images(QJsonObject const &jobj, QString const &mode)
    {
        QImage qimg;
        qimg.loadFromData(QByteArray::fromBase64(jobj["im"].toString().toLatin1()), "JPG");
        if(!qimg.isNull()){
            auto pose_est =
                predict_pose<estimate_many_pose_similarity_worker_results>(qimg, config_.confidence_, net_);
            auto results = search_.find_top_k(pose_est.points_);
            return {std::move(results), std::move(pose_est.qimg_)};
        }else{
            qDebug()<<"qimage is none for find_similar_images";
        }

        return {};
    }

    config_estimate_many_pose_similarity_worker const config_;
    movenet_single_pose_estimate net_;
    pose_similarity_search search_;
};

estimate_many_pose_similarity_worker::
    estimate_many_pose_similarity_worker(config_estimate_many_pose_similarity_worker const &config, QObject *parent) :
    flt::mm::frame_process_base_worker(5, parent),
    impl_{std::make_unique<impl>(config)}
{

}

estimate_many_pose_similarity_worker::~estimate_many_pose_similarity_worker()
{

}

void estimate_many_pose_similarity_worker::process_results(std::any input)
{
    static int count = 0;    
    auto const jobj = QJsonDocument::fromJson(std::any_cast<QString>(input).toLatin1()).object();
    auto const mode = jobj["mode"].toString();
    qDebug()<<"count = "<<++count<<", mode = "<<mode;
    if(mode == "add"){
        auto results = impl_->add(jobj, mode);
        emit send_process_results(std::move(results));
        emit send_msg("next");
    }else if(mode == "compare"){
        emit send_similar_pose(impl_->find_similar_images(jobj, mode));
    }else if(mode == "send_request_img"){
        QImage qimg;
        qimg.loadFromData(QByteArray::fromBase64(jobj["im"].toString().toLatin1()), "JPG");
        emit send_request_image(qimg);
    }else if(mode == "add_by_json"){
        auto results = impl_->add_by_json(jobj, mode);
        emit send_process_results(std::move(results));
        emit send_msg("next");
    }else{
        emit send_msg(mode);
    }
}
