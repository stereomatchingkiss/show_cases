#pragma once

#include "../obj_det_base.hpp"

namespace flt::cvt::det{

class yolox : public obj_det_base
{
public:
    yolox(const char* param,
          const char* bin,          
          bool use_gpu = false,
          int input_size = 416,
          int max_thread = 4);

    ~yolox();

    std::vector<box_info> predict(cv::Mat const &bgr,
                                  float score_threshold = 0.4f,
                                  float nms_threshold = 0.5f,
                                  int rotation_angle = 0,
                                  bool hflip = false) override;

private:
    ncnn::Net net_;
    int target_size_;
    int num_class_;

    std::string input_name_;
    std::string output_name_;
};

}
