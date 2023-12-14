#pragma once

#include <opencv2/core.hpp>

namespace flt::cvt::video{

class video_classify_base
{
public:
    video_classify_base();
    virtual ~video_classify_base();

    virtual std::vector<std::tuple<float, size_t>> predict(cv::Mat const &input, int top_k) = 0;
};

}
