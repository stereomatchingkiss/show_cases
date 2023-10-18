#include "nanodet_utils.hpp"

#include "nanodet.hpp"

#include "../utils/common_obj_det_type.hpp"

#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include <iostream>

namespace ocv{

namespace{

const int color_list[80][3] =
{
    //{255 ,255 ,255}, //bg
    {216 , 82 , 24},
    {236 ,176 , 31},
    {125 , 46 ,141},
    {118 ,171 , 47},
    { 76 ,189 ,237},
    {238 , 19 , 46},
    { 76 , 76 , 76},
    {153 ,153 ,153},
    {255 ,  0 ,  0},
    {255 ,127 ,  0},
    {190 ,190 ,  0},
    {  0 ,255 ,  0},
    {  0 ,  0 ,255},
    {170 ,  0 ,255},
    { 84 , 84 ,  0},
    { 84 ,170 ,  0},
    { 84 ,255 ,  0},
    {170 , 84 ,  0},
    {170 ,170 ,  0},
    {170 ,255 ,  0},
    {255 , 84 ,  0},
    {255 ,170 ,  0},
    {255 ,255 ,  0},
    {  0 , 84 ,127},
    {  0 ,170 ,127},
    {  0 ,255 ,127},
    { 84 ,  0 ,127},
    { 84 , 84 ,127},
    { 84 ,170 ,127},
    { 84 ,255 ,127},
    {170 ,  0 ,127},
    {170 , 84 ,127},
    {170 ,170 ,127},
    {170 ,255 ,127},
    {255 ,  0 ,127},
    {255 , 84 ,127},
    {255 ,170 ,127},
    {255 ,255 ,127},
    {  0 , 84 ,255},
    {  0 ,170 ,255},
    {  0 ,255 ,255},
    { 84 ,  0 ,255},
    { 84 , 84 ,255},
    { 84 ,170 ,255},
    { 84 ,255 ,255},
    {170 ,  0 ,255},
    {170 , 84 ,255},
    {170 ,170 ,255},
    {170 ,255 ,255},
    {255 ,  0 ,255},
    {255 , 84 ,255},
    {255 ,170 ,255},
    { 42 ,  0 ,  0},
    { 84 ,  0 ,  0},
    {127 ,  0 ,  0},
    {170 ,  0 ,  0},
    {212 ,  0 ,  0},
    {255 ,  0 ,  0},
    {  0 , 42 ,  0},
    {  0 , 84 ,  0},
    {  0 ,127 ,  0},
    {  0 ,170 ,  0},
    {  0 ,212 ,  0},
    {  0 ,255 ,  0},
    {  0 ,  0 , 42},
    {  0 ,  0 , 84},
    {  0 ,  0 ,127},
    {  0 ,  0 ,170},
    {  0 ,  0 ,212},
    {  0 ,  0 ,255},
    {  0 ,  0 ,  0},
    { 36 , 36 , 36},
    { 72 , 72 , 72},
    {109 ,109 ,109},
    {145 ,145 ,145},
    {182 ,182 ,182},
    {218 ,218 ,218},
    {  0 ,113 ,188},
    { 80 ,182 ,188},
    {127 ,127 ,  0},
};

}

void draw_bboxes(cv::Mat &image, const std::vector<utils::box_info> &bboxes, const std::vector<std::string> &class_names)
{
    for (size_t i = 0; i < bboxes.size(); i++){
        const utils::box_info& bbox = bboxes[i];
        cv::Scalar const color = cv::Scalar(color_list[bbox.label_][0], color_list[bbox.label_][1], color_list[bbox.label_][2]);
        cv::rectangle(image, cv::Point(bbox.x1_, bbox.y1_),cv::Point(bbox.x2_, bbox.y2_), color, 8);

        char text[256];
        sprintf(text, "%s %.1f%%", class_names[bbox.label_].c_str(), bbox.score_ * 100);

        int baseLine = 0;
        cv::Size label_size = cv::getTextSize(text, cv::FONT_HERSHEY_SIMPLEX, 0.4, 1, &baseLine);

        int x = (bbox.x1_);
        int y = bbox.y1_ - label_size.height - baseLine;
        if (y < 0)
            y = 0;
        if (x + label_size.width > image.cols)
            x = image.cols - label_size.width;

        cv::rectangle(image, cv::Rect(cv::Point(x, y), cv::Size(label_size.width, label_size.height + baseLine)),
                      color, -1);

        cv::putText(image, text, cv::Point(x, y + label_size.height),
                    cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(255, 255, 255), 4);//*/
    }
}

void draw_output_strings_results(cv::Mat &image, const std::vector<std::string> &outputs, const std::vector<std::string> &class_names)
{
    if(outputs.size() >= 6){
        std::vector<utils::box_info> results;
        for(size_t i = 0; i < outputs.size(); i += 6){
            utils::box_info info;
            info.x1_ = std::stoi(outputs[i]);
            info.y1_ = std::stoi(outputs[i + 1]);
            info.x2_ = std::stoi(outputs[i + 2]);
            info.y2_ = std::stoi(outputs[i + 3]);
            info.score_ = std::stof(outputs[i + 4]);
            info.label_ = std::stoi(outputs[i + 5]);
            results.emplace_back(info);
        }
        draw_bboxes(image, results, class_names);
    }
}

std::tuple<int, cv::Mat> predict_image(nanodet &net,
                                       const std::string &img_path,
                                       const std::vector<std::string> &names,
                                       float conf,
                                       float nms)
{
    if(auto img = cv::imread(img_path, cv::IMREAD_COLOR); !img.empty()){
        auto const boxes_info = net.predict(img, conf, nms);
        std::cout<<boxes_info.size()<<std::endl;
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
    utils::object_rect effect_roi;
    ocv::utils::resize_uniform(input, resize_img, effect_roi, input_size, input_size);

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
        nanodet::scale_bbox(input.rows, input.cols, boxes_info, effect_roi);
        for(auto const &val : boxes_info){
            predict_results += val.to_string(input.rows, input.cols);
        }
    }else{
        nanodet::scale_bbox(input.cols, input.rows, boxes_info, effect_roi);
        for(auto const &val : boxes_info){
            predict_results += val.to_string(input.cols, input.rows);
        }
    }

    predict_results.pop_back();

    return {0, std::move(predict_results)};
}

}
