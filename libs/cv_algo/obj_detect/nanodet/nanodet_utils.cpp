#include "nanodet_utils.hpp"

#include "nanodet.hpp"

#include "../common_obj_det_type.hpp"
#include "../obj_det_utils.hpp"

#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

namespace flt::cvt::det{

std::tuple<int, cv::Mat> predict_image(nanodet &net,
                                       const std::string &img_path,
                                       const std::vector<std::string> &names,
                                       float conf,
                                       float nms)
{
    if(auto img = cv::imread(img_path, cv::IMREAD_COLOR); !img.empty()){
        auto const boxes_info = net.predict(img, conf, nms);        
        draw_bboxes(img, boxes_info, names);
        cv::resize(img, img, cv::Size(480, 480));
        cv::imshow("img", img);
        auto const key = cv::waitKey();
        if(key == 'q' || key == 'Q'){
            return {1, img};
        }
        return {0, img};
    }

    return {-1, {}};
}

std::pair<int, std::string> predict_bboxes(cv::Mat const &input,
                                           nanodet &net,
                                           int rotate_angle,
                                           bool horizontal_flip,
                                           float conf_threshold,
                                           float nms_threshold)
{
    auto constexpr input_size = 320;
    cv::Mat resize_img;
    object_rect effect_roi;
    resize_uniform(input, resize_img, effect_roi, input_size, input_size);

    if(resize_img.channels() == 4){
        cv::cvtColor(resize_img, resize_img, cv::COLOR_RGBA2RGB);
    }
    auto boxes_info = net.predict_with_resize_image(resize_img, conf_threshold, nms_threshold,
                                                    rotate_angle, horizontal_flip);
    if(boxes_info.empty()){
        return {-4, ""};
    }

    std::string predict_results;
    auto const abs_rotate_angle = std::abs(rotate_angle);
    bool const should_swap = abs_rotate_angle == 90 || abs_rotate_angle == 270;
    if(should_swap){
        std::swap(effect_roi.height_, effect_roi.width_);
        std::swap(effect_roi.y_, effect_roi.x_);
        scale_bbox(input.rows, input.cols, boxes_info, effect_roi);
        for(auto const &val : boxes_info){
            predict_results += val.to_string(input.rows, input.cols);
        }
    }else{
        scale_bbox(input.cols, input.rows, boxes_info, effect_roi);
        for(auto const &val : boxes_info){
            predict_results += val.to_string(input.cols, input.rows);
        }
    }

    predict_results.pop_back();

    return {0, std::move(predict_results)};
}

}
