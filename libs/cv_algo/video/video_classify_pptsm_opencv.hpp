#pragma once

#include "video_classify_base.hpp"
#include "video_frame_extractor.hpp"

#include <opencv2/dnn.hpp>

namespace flt::cvt::video{

/**
 * @brief The video classify class based on the ppTSMV2
 */
class video_classify_pptsm_opencv : public video_classify_base
{
public:
    video_classify_pptsm_opencv(std::string const &weight_path, int sampling_rate = 25, int num_seg = 8);
    ~video_classify_pptsm_opencv();

    std::vector<std::tuple<float, size_t>> predict(cv::Mat const &input, int top_k = 1) override;

private:
    void softmax(float *begin, float *end) const noexcept;

    video_frame_extractor frame_extractor_;

    cv::dnn::Net net_;
    std::vector<std::tuple<float, size_t>> results_;
};

}
