#include "similarity_compare.hpp"

namespace flt::cvt::utils{

cv::Mat zscore(const cv::Mat &fc) {
    cv::Mat mean, std;
    meanStdDev(fc, mean, std);
    return ((fc - mean) / std);
}

float cosine_distance(cv::Mat const &v1, cv::Mat const &v2)
{
    double const dot = v1.dot(v2);
    double const denom_v1 = norm(v1);
    double const denom_v2 = norm(v2);

    return dot / (denom_v1 * denom_v2);
}

float cosine_distance_zscore(cv::Mat const &v1, cv::Mat const &v2)
{
    return cosine_distance(zscore(v1), zscore(v2));
}

float cosine_distance(const std::vector<float> &v1, const std::vector<float> &v2)
{
    cv::Mat mat_0(1, static_cast<int>(v1.size()), CV_32F, const_cast<float*>(v1.data()));
    cv::Mat mat_1(1, static_cast<int>(v2.size()), CV_32F, const_cast<float*>(v2.data()));

    return cosine_distance(mat_0, mat_1);
}

float cosine_distance_zscore(const std::vector<float> &v1, const std::vector<float> &v2)
{
    cv::Mat mat_0(1, static_cast<int>(v1.size()), CV_32F, const_cast<float*>(v1.data()));
    cv::Mat mat_1(1, static_cast<int>(v2.size()), CV_32F, const_cast<float*>(v2.data()));

    return cosine_distance(zscore(mat_0), zscore(mat_1));
}

} // namespace flt::cvt::utils
