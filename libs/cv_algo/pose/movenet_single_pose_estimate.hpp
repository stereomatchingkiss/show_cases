#ifndef MOVENET_SINGLE_POSE_ESTIMATE_2342423
#define MOVENET_SINGLE_POSE_ESTIMATE_2342423

#include "pose_estimation_results.hpp"

#include <opencv2/core/core.hpp>

#include <net.h>

#include <memory>

namespace flt::cvt::pose{

class movenet_single_pose_estimate
{
public:
    movenet_single_pose_estimate(std::string const &param,
            std::string const &bin,
            int target_size = 192);
    ~movenet_single_pose_estimate();

    std::vector<keypoint> predict(cv::Mat &rgb);

    int draw(cv::Mat& rgb);

private:
    struct pimpl;

    std::unique_ptr<pimpl> pimpl_;
};

}

#endif // MOVENET_SINGLE_POSE_ESTIMATE_2342423
