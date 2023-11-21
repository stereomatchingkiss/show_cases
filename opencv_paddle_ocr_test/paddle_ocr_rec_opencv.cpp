#include "paddle_ocr_rec_opencv.hpp"

#include "crnn_process.h"

#include <format>
#include <fstream>
#include <iostream>

namespace{

inline static size_t arg_max(std::forward_iterator auto first, std::forward_iterator auto last) {
    return std::distance(first, std::max_element(first, last));
}

std::vector<std::string> read_keys(std::string const &fname)
{
    std::vector<std::string> keys_vec;
    if(std::ifstream in(fname); in.is_open()){
        std::string line;
        while(getline(in, line)){
            keys_vec.emplace_back(line);
        }
    }

    return keys_vec;
}

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

}

paddle_ocr_rec_opencv::paddle_ocr_rec_opencv(const std::string &model_weights, std::string const &key_files, int dst_height) :
    dist_height_{dst_height},
    keys_{read_keys(key_files)},
    net_(cv::dnn::readNet(model_weights))
{

}

paddle_ocr_rec_opencv::~paddle_ocr_rec_opencv()
{

}

std::string paddle_ocr_rec_opencv::predict(const cv::Mat &mat, const std::vector<std::vector<std::vector<int> > > &boxes)
{
    for(size_t i = 0; i != boxes.size(); ++i){

        auto crop_img = GetRotateCropImage(mat, boxes[i]);
        float const wh_ratio = static_cast<float>(crop_img.cols) / static_cast<float>(crop_img.rows);
        auto resize_img = CrnnResizeImg(crop_img, wh_ratio, dist_height_);
        resize_img = cv::dnn::blobFromImage(resize_img, 1.0f / 255.0f, resize_img.size(), cv::Scalar(127.5, 127.5, 127.5), false, false);
        {
            auto ptr = resize_img.ptr<float>(0);
            for(size_t i = 0; i != resize_img.total(); i += 3){
                for(size_t j = 0; j != 3; ++j){
                    ptr[i +j] *= 1/0.5f;
                }
            }
        }
        std::cout<<resize_img.size<<std::endl;

        net_.setInput(resize_img);
        std::vector<cv::Mat> detections;
        net_.forward(detections, getOutputsNames(net_));

        std::cout<<"detections = "<<detections.size()<<std::endl;//*/
    }

    return {};
}
