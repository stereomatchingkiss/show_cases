#ifndef MOVENET_SINGLE_POSE_ESTIMATE
#define MOVENET_SINGLE_POSE_ESTIMATE

#include <opencv2/core.hpp>

#include <memory>
#include <string>

namespace flt::cvt::pose{

struct pose_estimation_results;

class movenet_single_pose_estimate
{
public:
    explicit movenet_single_pose_estimate(std::string const &param,
                                          std::string const &bin,
                                          int min_valid_keypoints = 4,
                                          float min_confidence = 0.2f,
                                          int target_size = 256);
    ~movenet_single_pose_estimate();

    pose_estimation_results predict(cv::Mat const &bgr);

    float get_min_confidence() const noexcept;
    int get_min_valid_keypoints() const noexcept;    

    void set_min_confidence(float val);
    void set_min_valid_keypoints(int val);    

private:
    struct pimpl;

    std::unique_ptr<pimpl> pimpl_;
};

}

#endif


