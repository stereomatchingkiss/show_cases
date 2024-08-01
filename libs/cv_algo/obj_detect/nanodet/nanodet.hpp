//
// Create by RangiLyu
// 2020 / 10 / 2
//

#pragma once

#include "../common_obj_det_type.hpp"
#include "../obj_det_base.hpp"

#include <opencv2/core.hpp>

#ifdef THIS_IS_IOS
#include <ncnn/ncnn/net.h>
#else
#include <net.h>
#endif

#include <memory>
#include <vector>

namespace flt::cvt::det{

class nanodet_raw_ncnn;

/**
 * @brief nanodet class designed for unity
 */
class nanodet : public obj_det_base
{
public:    
    nanodet(const char* param,
            const char* bin,
            int num_class,
            bool use_gpu = false,
            int input_size = 320,
            int max_thread = 4);

    ~nanodet();

    bool get_load_model_success() const noexcept;
    bool get_load_param_success() const noexcept;
    int get_load_model_state() const noexcept;
    //To fix : too many params
    std::vector<box_info> predict_with_resize_image(cv::Mat const &bgr,
                                                    float score_threshold,
                                                    float nms_threshold,
                                                    int rotation_angle,
                                                    bool hflip);
    std::vector<box_info> predict(cv::Mat const &bgr,
                                  float score_threshold,
                                  float nms_threshold,
                                  int rotation_angle = 0,
                                  bool hflip = false) override;

private:
    std::unique_ptr<nanodet_raw_ncnn> net_;
};

}
