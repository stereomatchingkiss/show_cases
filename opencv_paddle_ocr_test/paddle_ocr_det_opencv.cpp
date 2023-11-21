#include "paddle_ocr_det_opencv.hpp"

#include "db_post_process.h"

#include <cv_algo/ocr/paddle_ocr/common.hpp>

#include <opencv2/core.hpp>
//#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/dnn.hpp>

#include <format>
#include <map>
#include <iostream>

namespace{

// Get Output Layers Name
std::vector<std::string> getOutputsNames( const cv::dnn::Net& net )
{
    static std::vector<std::string> names;
    if( names.empty() ){
        std::vector<int32_t> out_layers = net.getUnconnectedOutLayers();
        std::vector<std::string> layers_names = net.getLayerNames();
        names.resize( out_layers.size() );
        for( size_t i = 0; i < out_layers.size(); ++i ){
            names[i] = layers_names[out_layers[i] - 1];
        }
    }

    for(auto const &val : names){
        std::cout<<"output name = "<<val<<std::endl;
    }

    return names;
}


// resize image to a size multiple of 32 which is required by the network
cv::Mat DetResizeImg(const cv::Mat img,
                     int max_size_len,
                     std::array<float, 3> &ratio_hw) {
    int w = img.cols;
    int h = img.rows;

    ratio_hw[2] = 1.f;
    int max_wh = w >= h ? w : h;
    if (max_wh > max_size_len) {
        if (h > w) {
            ratio_hw[2] = static_cast<float>(max_size_len) / static_cast<float>(h);
        } else {
            ratio_hw[2] = static_cast<float>(max_size_len) / static_cast<float>(w);
        }
    }

    int resize_h = static_cast<int>(float(h) * ratio_hw[2]);
    int resize_w = static_cast<int>(float(w) * ratio_hw[2]);
    if (resize_h / 32 < 1 + 1e-5)
        resize_h = 32;
    else
        resize_h = (resize_h / 32 - 1) * 32;

    if (resize_w / 32 < 1 + 1e-5)
        resize_w = 32;
    else
        resize_w = (resize_w / 32 - 1) * 32;

    cv::Mat resize_img;
    cv::resize(img, resize_img, cv::Size(resize_w, resize_h));

    ratio_hw[0] = (static_cast<float>(resize_h) / static_cast<float>(h));
    ratio_hw[1] = (static_cast<float>(resize_w) / static_cast<float>(w));

    return resize_img;
}

}

paddle_ocr_det_opencv::paddle_ocr_det_opencv(const std::string &model_weights,
                                             float binary_map_thresh,
                                             float box_thresh,
                                             float unclip_ratio,
                                             bool use_dilate) :
    binary_map_thresh_{binary_map_thresh},
    box_thresh_{box_thresh},
    unclip_ratio_{unclip_ratio},
    use_dilate_{use_dilate},
    net_{cv::dnn::readNet(model_weights)}
{
    net_ = cv::dnn::readNet(model_weights);
}

paddle_ocr_det_opencv::~paddle_ocr_det_opencv()
{

}

std::vector<flt::cvt::ocr::TextBox> paddle_ocr_det_opencv::predict(const cv::Mat &input)
{
    std::map<std::string, double> config{
        {"det_db_box_thresh", 0.5},
        {"det_db_thresh", 0.3},
        {"det_db_unclip_ratio", 1.6},
        {"det_db_use_dilate", 0},
        {"det_use_polygon_score", 0},
        {"max_side_len", 960},
        {"rec_image_height", 48},
        {"use_direction_classify", 1}
    };

    return RunDetModel(net_, input, config);
}

std::vector<flt::cvt::ocr::TextBox> paddle_ocr_det_opencv::RunDetModel(cv::dnn::Net &net,
                                                                              cv::Mat const &img,
                                                                              std::map<std::string, double> &Config)
{
    // Read img
    int constexpr max_side_len = 960;
    int const origin_row = img.rows;
    int const origin_col = img.cols;

    std::array<float, 3> ratio_hw;
    auto resize_img = DetResizeImg(img, max_side_len, ratio_hw);

    cv::Mat img_fp = cv::dnn::blobFromImage(resize_img, 1.0f / 255.0f, resize_img.size(), cv::Scalar(123, 117, 104), false, false);
    {
        auto ptr = img_fp.ptr<float>(0);
        float constexpr scale[] = {1 / 0.229f, 1 / 0.224f, 1 / 0.225f};
        for(size_t i = 0; i != img_fp.total(); i += 3){
            for(size_t j = 0; j != 3; ++j){
                ptr[i +j] *= scale[j];
            }
        }
    }

    net.setInput(img_fp);

    // Run Forward Network
    std::vector<cv::Mat> detections;
    net.forward( detections, getOutputsNames( net ) );

    auto const &det_out = detections[0];
    auto const shape_out = det_out.size;

    cv::Mat cbuf_map(shape_out[2], shape_out[3], CV_8UC1);
    cv::Mat pred_map(shape_out[2], shape_out[3], CV_32F, det_out.data);

    auto outptr = det_out.ptr<float>(0);
    auto cbuf_map_ptr = cbuf_map.ptr<uchar>(0);
    auto pred_map_ptr = pred_map.ptr<float>(0);
    for(int i = 0; i < int(shape_out[2] * shape_out[3]); ++i) {
        pred_map_ptr[i] = static_cast<float>(outptr[i]);
        cbuf_map_ptr[i] = static_cast<unsigned char>((outptr[i]) * 255);
    }

    //binary_map_thresh
    const double threshold = double(Config["det_db_thresh"]) * 255;
    const double max_value = 255;
    cv::Mat bit_map;
    cv::threshold(cbuf_map, bit_map, threshold, max_value, cv::THRESH_BINARY);

    if(use_dilate_){
        cv::Mat dilation_map;
        cv::Mat const dila_ele = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(2, 2));
        cv::dilate(bit_map, dilation_map, dila_ele);
        bit_map = dilation_map;
    }

    auto boxes = BoxesFromBitmap(pred_map, bit_map, box_thresh_, unclip_ratio_);

    return FilterTagDetRes(boxes, ratio_hw[0], ratio_hw[1], origin_row, origin_col);
}
