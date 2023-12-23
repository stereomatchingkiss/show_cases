#include "face_warp.hpp"

#include <opencv2/imgproc.hpp>

#include <numbers>

namespace flt::cvt::face{

face_warp::face_warp(int target_size) :
    target_warp_points_{
                        {30.2946f, 51.6963f},
                        {65.5318f, 51.5014f},
                        {48.0252f, 71.7366f},
                        {33.5493f, 92.3655f},
                        {62.7299f, 92.2041f}},
    target_face_landmarks_(5, 2, CV_32F, target_warp_points_.data()),
    target_size_{target_size}
{    
}

face_warp::~face_warp()
{

}

cv::Mat face_warp::process(cv::Mat const &input, std::vector<cv::Point2f> const &landmarks) const
{
    cv::Mat dst(5, 2, CV_32FC1, const_cast<cv::Point2f*>(landmarks.data()));
    cv::Mat aligned;
    cv::Mat m = similar_transform(dst, target_face_landmarks_);
    cv::warpPerspective(input, aligned, m, cv::Size(96, 112), cv::INTER_LINEAR);

    cv::resize(aligned, aligned, cv::Size(target_size_, target_size_), 0, 0, cv::INTER_LINEAR);

    return aligned;
}

int face_warp::matrix_rank(cv::Mat const &M) const
{
    cv::Mat w, u, vt;
    cv::SVD::compute(M, w, u, vt);
    cv::Mat1b const nonZeroSingularValues = w > 0.0001;

    return countNonZero(nonZeroSingularValues);
}

cv::Mat face_warp::var_axis_0(const cv::Mat &src) const
{
    cv::Mat temp = element_wise_minus(src,mean_axis_0(src));
    cv::multiply(temp ,temp ,temp );

    return mean_axis_0(temp);
}

cv::Mat face_warp::mean_axis_0(const cv::Mat &src) const
{
    int const num = src.rows;
    int const dim = src.cols;

    // x1 y1
    // x2 y2
    cv::Mat output(1, dim, CV_32F);
    auto out_ptr = output.ptr<float>();
    for(int i = 0 ; i <  dim; ++i){
        float sum = 0;
        for(int j = 0; j < num ; ++j){
            sum += src.at<float>(j,i);
        }
        out_ptr[i] = sum/num;
    }

    return output;
}

cv::Mat face_warp::element_wise_minus(cv::Mat const &A, cv::Mat const &B) const
{
    cv::Mat output(A.rows, A.cols, A.type());
    if(B.cols == A.cols){
        auto b_ptr = B.ptr<float>();
        for(int i = 0 ; i <  A.rows; i ++){
            auto out_ptr = output.ptr<float>(i);
            auto a_ptr = A.ptr<float>(i);
            for(int j = 0 ; j < B.cols; j++){
                out_ptr[j] = a_ptr[j] - b_ptr[j];
            }
        }
    }
    return output;
}

cv::Mat face_warp::similar_transform(cv::Mat const &src, cv::Mat const &dst) const
{
    int const num = src.rows;
    int const dim = src.cols;
    cv::Mat const src_mean = mean_axis_0(src);
    cv::Mat const dst_mean = mean_axis_0(dst);
    cv::Mat const src_demean = element_wise_minus(src, src_mean);
    cv::Mat const dst_demean = element_wise_minus(dst, dst_mean);
    cv::Mat const A = (dst_demean.t() * src_demean) / static_cast<float>(num);
    cv::Mat d(dim, 1, CV_32F);
    d.setTo(1.0f);
    if(cv::determinant(A) < 0) {
        d.at<float>(dim - 1, 0) = -1;
    }
    cv::Mat const T = cv::Mat::eye(dim + 1, dim + 1, CV_32F);
    cv::Mat U, S, V;

    // the SVD function in opencv differ from scipy .
    cv::SVD::compute(A, S,U, V);

    if(int const rank = matrix_rank(A); rank == dim - 1) {
        if(cv::determinant(U) * cv::determinant(V) > 0) {
            T.rowRange(0, dim).colRange(0, dim) = U * V;
        }else{
            d.at<float>(dim - 1, 0) = -1;
            int const s = -1;
            T.rowRange(0, dim).colRange(0, dim) = U * V;
            cv::Mat const diag_ = cv::Mat::diag(d);
            cv::Mat const twp = diag_*V; //np.dot(np.diag(d), V.T)
            cv::Mat const B = cv::Mat::zeros(3, 3, CV_8UC1);
            cv::Mat const C = B.diag(0);
            T.rowRange(0, dim).colRange(0, dim) = U* twp;
            d.at<float>(dim - 1, 0) = s;
        }
    }else{
        cv::Mat const diag_ = cv::Mat::diag(d);
        cv::Mat const twp = diag_*V.t(); //np.dot(np.diag(d), V.T)
        cv::Mat const res = U* twp; // U
        T.rowRange(0, dim).colRange(0, dim) = -U.t()* twp;
    }

    cv::Mat const var_ = var_axis_0(src_demean);
    double const val = cv::sum(var_).val[0];
    cv::Mat res;
    cv::multiply(d,S,res);
    float const scale =  static_cast<float>(1.0/val*cv::sum(res).val[0]);
    T.rowRange(0, dim).colRange(0, dim) = - T.rowRange(0, dim).colRange(0, dim).t();
    cv::Mat const temp1 = T.rowRange(0, dim).colRange(0, dim); // T[:dim, :dim]
    cv::Mat const temp2 = src_mean.t(); //src_mean.T
    cv::Mat const temp3 = temp1*temp2; // np.dot(T[:dim, :dim], src_mean.T)
    cv::Mat const temp4 = scale*temp3;
    T.rowRange(0, dim).colRange(dim, dim+1)=  -(temp4 - dst_mean.t()) ;
    T.rowRange(0, dim).colRange(0, dim) *= scale;
    return T;
}

}
