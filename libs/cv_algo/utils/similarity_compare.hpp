#pragma once

#include <opencv2/core.hpp>

namespace flt::cvt::utils{

cv::Mat zscore(const cv::Mat &fc);

float cosine_distance(std::vector<float> const &v1, std::vector<float> const &v2);
float cosine_distance(cv::Mat const &v1, cv::Mat const &v2);

float cosine_distance_zscore(std::vector<float> const &v1, std::vector<float> const &v2);
float cosine_distance_zscore(cv::Mat const &v1, cv::Mat const &v2);

}
