#include "pose_similarity_estimation.hpp"

namespace flt::cvt::pose{

float euclidean_distance(const std::vector<keypoint> &kpts0, const std::vector<keypoint> &kpts1, float normalize)
{
    float sum = 0;
    for(size_t i = 0; i != kpts0.size(); ++i){
        sum += std::pow(kpts0[i].hx_ / normalize - kpts1[i].hx_ / normalize, 2.0f) +
               std::pow(kpts0[i].hy_ / normalize - kpts1[i].hy_ / normalize, 2.0f);
    }

    return std::sqrt(sum);
}

}
