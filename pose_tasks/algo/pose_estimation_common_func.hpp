#pragma once

#include <cv_algo/pose/movenet_single_pose_estimate.hpp>
#include <cv_algo/pose/pose_estimation_utils.hpp>
#include <utils/qimage_to_cvmat.hpp>

#include <opencv2/imgproc.hpp>

#include <concepts>

template<typename T, typename U>
requires std::is_class_v<T> && std::is_class_v<U>
T predict_pose(U &inout, float confidence, flt::cvt::pose::movenet_single_pose_estimate &net)
{
    auto [mat, non_copy] = flt::qimg_convert_to_cvmat_non_copy(inout.qimg_);
    cv::cvtColor(mat, mat, cv::COLOR_BGR2RGB);

    T results;
    results.is_target_ = inout.is_target_;
    results.points_ = net.predict(mat);
    flt::cvt::pose::draw(mat, results.points_, confidence);
    if(non_copy){
        results.qimg_ = inout.qimg_;
    }else{
        //Todo : reduce useless operations
        results.qimg_ = QImage(mat.data, mat.cols, mat.rows, mat.step[0], QImage::Format_RGB888).copy();
    }

    return results;
}
