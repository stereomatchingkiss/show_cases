#ifndef NANODET_RAW_NCNN_HPP
#define NANODET_RAW_NCNN_HPP

#include "../../utils/common_obj_det_type.hpp"

#ifdef THIS_IS_IOS
#include <ncnn/ncnn/net.h>
#else
#include <net.h>
#endif

#include <memory>

/**
 * This class do not depends on opencv, easier to create plugin for python
 */

namespace ocv{

class nanodet_raw_ncnn
{
public:
    nanodet_raw_ncnn(const char* param,
                     const char* bin,
                     int num_class,
                     bool use_gpu = false,
                     int input_size = 320,
                     int max_thread = 4);

    int get_input_size() const;

    bool get_load_model_success() const noexcept;
    bool get_load_param_success() const noexcept;
    int get_load_model_state() const noexcept;
    //To fix : too many params
    /**
     * @brief predict_with_resize_image
     * @param buffer
     * @param width
     * @param height
     * @param score_threshold
     * @param nms_threshold
     * @param rotation_angle
     * @param hflip
     * @return
     * @todo too many params
     */
    std::vector<utils::box_info> predict_with_resize_image(unsigned char *buffer,
                                                           int width,
                                                           int height,
                                                           float score_threshold,
                                                           float nms_threshold);

    static void scale_bbox(int src_w, int src_h, std::vector<utils::box_info>& bboxes, utils::object_rect const &effect_roi);

private:
    struct HeadInfo
    {
        std::string cls_layer;
        std::string dis_layer;
        int stride;
    };

    std::vector<HeadInfo> const heads_info{
        // cls_pred|dis_pred|stride
        {"cls_pred_stride_8", "dis_pred_stride_8", 8},
        {"cls_pred_stride_16", "dis_pred_stride_16", 16},
        {"cls_pred_stride_32", "dis_pred_stride_32", 32},
    };

    ncnn::Extractor create_extractor() const;
    void decode_infer(ncnn::Mat& feats,
                      std::vector<utils::center_prior>& center_priors,
                      float threshold,
                      std::vector<std::vector<utils::box_info>>& results);
    utils::box_info dis_pred_to_box(const float*& dfl_det, int label, float score, int x, int y, int stride) const;
    static void nms(std::vector<utils::box_info>& input_boxes, float nms_threshold);
    void preprocess(unsigned char *buffer, int width, int height, ncnn::Mat& in) const;

    bool has_gpu_ = false;
    int const input_size_[2] = {320, 320};
    std::unique_ptr<ncnn::Net> net_;
    int load_model_success_ = -1;
    int load_param_success_ = -1;
    int max_thread_ = 4;
    int num_class_ = 7;
    int reg_max_ = 7;
    std::vector<int> const strides_ = { 8, 16, 32, 64 }; // strides of the multi-level feature.
};

}

#endif // NANODET_RAW_NCNN_HPP
