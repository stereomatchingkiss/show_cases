#pragma once

#include <cv_algo/pose/movenet_single_pose_estimate.hpp>
#include <cv_algo/pose/pose_estimation_utils.hpp>
#include <utils/qimage_to_cvmat.hpp>

#include <opencv2/imgproc.hpp>

#include <concepts>

class QJsonObject;

template<typename T>
requires std::is_class_v<T>
T predict_pose(QImage &qimg, float confidence, flt::cvt::pose::movenet_single_pose_estimate &net)
{
    auto [mat, non_copy] = flt::qimg_convert_to_cvmat_non_copy(qimg);
    cv::cvtColor(mat, mat, cv::COLOR_BGR2RGB);

    T results;    
    results.points_ = net.predict(mat);
    flt::cvt::pose::draw(mat, results.points_, confidence);
    if(non_copy){
        results.qimg_ = qimg;
    }else{
        //Todo : reduce useless operations
        results.qimg_ = QImage(mat.data, mat.cols, mat.rows, mat.step[0], QImage::Format_RGB888).copy();
    }

    return results;
}

QJsonObject convert_to_json(std::vector<flt::cvt::pose::keypoint> const &input);

std::string pose_model_root();

std::string pose_bin_path();

std::string pose_param_path();
