#pragma once

#include <opencv2/core.hpp>

#include <memory>
#include <string>

namespace flt::cvt::face{

class face_recognition_arcface_ncnn
{
public:
    explicit face_recognition_arcface_ncnn(std::string const &param, std::string const &bin, int target_size = 112);
    ~face_recognition_arcface_ncnn();

    std::vector<float> predict(cv::Mat const &input);

private:
    struct impl;
    std::unique_ptr<impl> impl_;
};

}
