#pragma once

#include "pose_estimation_results.hpp"

#include <opencv2/core.hpp>

namespace flt::cvt::pose{

float euclidean_distance(std::vector<keypoint> const &kpts0, std::vector<keypoint> const &kpts1, float normalize = 64.0f);

}
