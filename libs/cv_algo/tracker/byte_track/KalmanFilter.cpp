#include "KalmanFilter.hpp"

#include <cstddef>

namespace flt::cvt::tracker{

KalmanFilter::KalmanFilter(float std_weight_position, float std_weight_velocity) :
    std_weight_position_(std_weight_position),
    std_weight_velocity_(std_weight_velocity)
{
    constexpr size_t ndim = 4;
    constexpr float dt = 1;

    motion_mat_ = Eigen::MatrixXf::Identity(8, 8);
    update_mat_ = Eigen::MatrixXf::Identity(4, 8);

    for (size_t i = 0; i < ndim; i++)
    {
        motion_mat_(i, ndim + i) = dt;
    }
}

void KalmanFilter::initiate(StateMean &mean, StateCov &covariance, const DetectBox &measurement)
{
    mean.block<1, 4>(0, 0) = measurement.block<1, 4>(0, 0);
    mean.block<1, 4>(0, 4) = Eigen::Vector4f::Zero();

    StateMean std;
    std(0) = 2 * std_weight_position_ * measurement[3];
    std(1) = 2 * std_weight_position_ * measurement[3];
    std(2) = 1e-2f;
    std(3) = 2 * std_weight_position_ * measurement[3];
    std(4) = 10 * std_weight_velocity_ * measurement[3];
    std(5) = 10 * std_weight_velocity_ * measurement[3];
    std(6) = 1e-5f;
    std(7) = 10 * std_weight_velocity_ * measurement[3];

    StateMean tmp = std.array().square();
    covariance = tmp.asDiagonal();
}

void KalmanFilter::predict(StateMean &mean, StateCov &covariance)
{
    StateMean std;
    std(0) = std_weight_position_ * mean(3);
    std(1) = std_weight_position_ * mean(3);
    std(2) = 1e-2f;
    std(3) = std_weight_position_ * mean(3);
    std(4) = std_weight_velocity_ * mean(3);
    std(5) = std_weight_velocity_ * mean(3);
    std(6) = 1e-5f;
    std(7) = std_weight_velocity_ * mean(3);

    StateMean tmp = std.array().square();
    StateCov motion_cov = tmp.asDiagonal();

    mean = motion_mat_ * mean.transpose();
    covariance = motion_mat_ * covariance * (motion_mat_.transpose()) + motion_cov;
}

void KalmanFilter::update(StateMean &mean, StateCov &covariance, const DetectBox &measurement)
{
    StateHMean projected_mean;
    StateHCov projected_cov;
    project(projected_mean, projected_cov, mean, covariance);

    Eigen::Matrix<float, 4, 8> B = (covariance * (update_mat_.transpose())).transpose();
    Eigen::Matrix<float, 8, 4> kalman_gain = (projected_cov.llt().solve(B)).transpose();
    Eigen::Matrix<float, 1, 4> innovation = measurement - projected_mean;

    const auto tmp = innovation * (kalman_gain.transpose());
    mean = (mean.array() + tmp.array()).matrix();
    covariance = covariance - kalman_gain * projected_cov * (kalman_gain.transpose());
}

void KalmanFilter::project(StateHMean &projected_mean, StateHCov &projected_covariance,
                           const StateMean& mean, const StateCov& covariance)
{
    DetectBox std;
    std << std_weight_position_ * mean(3),
            std_weight_position_ * mean(3),
            1e-1f,
            std_weight_position_ * mean(3);

    projected_mean = update_mat_ * mean.transpose();
    projected_covariance = update_mat_ * covariance * (update_mat_.transpose());

    Eigen::Matrix<float, 4, 4> diag = std.asDiagonal();
    projected_covariance += diag.array().square().matrix();
}

}
