#include "estimate_many_pose_similarity_worker.hpp"

#include <cv_algo/pose/movenet_single_pose_estimate.hpp>
#include <cv_algo/pose/pose_estimation_utils.hpp>
#include <cv_algo/pose/pose_similarity_estimation.hpp>
#include <utils/qimage_to_cvmat.hpp>

//#include "estimate_pose_similarity_worker_input.hpp"
//#include "estimate_pose_similarity_worker_results.hpp"

#include "../config/config_estimate_many_pose_similarity_worker.hpp"

#include <QDebug>

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

#include <opencv2/imgproc.hpp>

using namespace flt::cvt::pose;

struct estimate_many_pose_similarity_worker::impl
{
    impl(config_estimate_many_pose_similarity_worker const &config) :
        config_{config}
    {

    }

    config_estimate_many_pose_similarity_worker const config_;
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
    qDebug()<<"count = "<<++count;
    auto const contents = std::any_cast<QString>(input);
    if(contents != "start"){
        qDebug()<<"contents size = "<<contents.size();
        auto const jobj = QJsonDocument::fromJson(contents.toLatin1()).object();
        qDebug()<<"im path = "<<jobj["im_path"].toString();
        emit send_msg("next");
    }else{
        qDebug()<<"can start : ";
        emit send_msg("start");
    }
}
