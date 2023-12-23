#pragma once

#include <opencv2/core.hpp>

namespace flt::cvt::face{

class face_recognition_base
{
public:
    face_recognition_base();
    virtual ~face_recognition_base();

    virtual std::vector<float> predict(cv::Mat const &input) = 0;
};

}
