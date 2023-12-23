#pragma once

#include "../face_detector_base.hpp"
#include "../face_detector_box.hpp"

#include <memory>
#include <string>

namespace flt::cvt::face{

class face_detector_retina_face_ncnn : public face_detector_base
{
public:
    face_detector_retina_face_ncnn(std::string const &param, std::string const &bin, float nms_threshold = 0.4);
    ~face_detector_retina_face_ncnn();

    int get_load_model_state() const noexcept;

    /**
     * @param bgr Input need to resize to 300*300
     * @return
     */
    std::vector<face_detector_box> predict(cv::Mat const &bgr) override;

private:
    struct impl;
    std::unique_ptr<impl> impl_;
};

}
