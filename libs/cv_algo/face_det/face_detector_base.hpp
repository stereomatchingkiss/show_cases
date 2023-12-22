#pragma once

#include "face_detector_box.hpp"

#include <opencv2/core.hpp>

namespace flt::cvt::face{

class face_detector_base
{
public:
    face_detector_base();

    virtual std::vector<face_detector_box> predict(cv::Mat const &input) = 0;
};

}
