#include "obj_det_utils.hpp"

#include "common_obj_det_type.hpp"

#include <format>

#include <opencv2/imgproc.hpp>

namespace ocv::det{

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

void draw_bboxes(cv::Mat &image,
                 const std::vector<box_info> &bboxes,
                 const std::vector<std::string> &class_names)
{
    for(size_t i = 0; i < bboxes.size(); i++){
        const box_info& bbox = bboxes[i];
        std::string text;
        if(bbox.track_id_ == -1){
            text = std::format("{} {:.1f}", class_names[bbox.label_], bbox.score_ * 100);
        }else{
            text = std::format("{} {:.1f}, {}", class_names[bbox.label_], bbox.score_ * 100, bbox.track_id_);
        }
        draw_bboxes_custom(image, bboxes[i], text);
    }
}

void draw_bboxes_custom(cv::Mat &image, const box_info &bboxes, const std::string &message)
{
    const box_info& bbox = bboxes;
    cv::Scalar const color = cv::Scalar(color_list[bbox.label_][0], color_list[bbox.label_][1], color_list[bbox.label_][2]);
    cv::rectangle(image,
                  cv::Point(static_cast<int>(bbox.rect_.x), static_cast<int>(bbox.rect_.y)),
                  cv::Point(static_cast<int>(bbox.x2_), static_cast<int>(bbox.y2_)),
                  color, 8);

    int baseLine = 0;
    cv::Size label_size = cv::getTextSize(message, cv::FONT_HERSHEY_SIMPLEX, 0.4, 1, &baseLine);

    int x = static_cast<int>(bbox.rect_.x);
    int y = static_cast<int>(bbox.rect_.y - label_size.height - baseLine);
    if (y < 0)
        y = 0;
    if (x + label_size.width > image.cols)
        x = image.cols - label_size.width;

    cv::rectangle(image, cv::Rect(cv::Point(x, y), cv::Size(label_size.width, label_size.height + baseLine)),
                  color, -1);

    cv::putText(image, message, cv::Point(x, y + label_size.height),
                cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(255, 255, 255), 4);
}

void draw_bboxes_custom(cv::Mat &image, const std::vector<box_info> &bboxes, const std::vector<std::string> &message)
{
    for (size_t i = 0; i < bboxes.size(); i++){
        draw_bboxes_custom(image, bboxes[i], message[i]);
    }
}

void draw_output_strings_results(cv::Mat &image, const std::vector<std::string> &outputs, const std::vector<std::string> &class_names)
{
    if(outputs.size() >= 6){
        std::vector<box_info> results;
        for(size_t i = 0; i < outputs.size(); i += 6){
            box_info info;
            info.rect_.x = static_cast<float>(std::stoi(outputs[i]));
            info.rect_.y = static_cast<float>(std::stoi(outputs[i + 1]));
            info.x2_ = static_cast<float>(std::stoi(outputs[i + 2]));
            info.y2_ = static_cast<float>(std::stoi(outputs[i + 3]));
            info.score_ = std::stof(outputs[i + 4]);
            info.label_ = std::stoi(outputs[i + 5]);
            results.emplace_back(info);
        }
        draw_bboxes(image, results, class_names);
    }
}

int resize_uniform(const cv::Mat &src, cv::Mat &dst, object_rect &effect_area, int dst_w, int dst_h)
{
    int const w = src.cols;
    int const h = src.rows;
    //std::cout << "src: (" << h << ", " << w << ")" << std::endl;
    dst = cv::Mat(cv::Size(dst_w, dst_h), CV_8UC3, cv::Scalar(0));

    float ratio_src = w * 1.0f / h;
    float ratio_dst = dst_w * 1.0f / dst_h;

    int tmp_w = 0;
    int tmp_h = 0;
    if (ratio_src > ratio_dst) {
        tmp_w = dst_w;
        tmp_h = static_cast<int>(std::floor((dst_w * 1.0f / w) * h));
    }
    else if (ratio_src < ratio_dst) {
        tmp_h = dst_h;
        tmp_w = static_cast<int>(std::floor((dst_h * 1.0f / h) * w));
    }
    else {
        cv::resize(src, dst, cv::Size(dst_w, dst_h));
        effect_area.x_ = 0;
        effect_area.y_ = 0;
        effect_area.width_ = dst_w;
        effect_area.height_ = dst_h;
        return 0;
    }

    //std::cout << "tmp: (" << tmp_h << ", " << tmp_w << ")" << std::endl;
    cv::Mat tmp;
    cv::resize(src, tmp, cv::Size(tmp_w, tmp_h));

    if (tmp_w != dst_w) {
        int index_w = static_cast<int>(std::floor((dst_w - tmp_w) / 2.0f));
        //std::cout << "index_w: " << index_w << std::endl;
        for (int i = 0; i < dst_h; i++) {
            memcpy(dst.data + i * dst_w * 3 + index_w * 3, tmp.data + i * tmp_w * 3, tmp_w * 3);
        }
        effect_area.x_ = index_w;
        effect_area.y_ = 0;
        effect_area.width_ = tmp_w;
        effect_area.height_ = tmp_h;
    }
    else if (tmp_h != dst_h) {
        int index_h = static_cast<int>(std::floor((dst_h - tmp_h) / 2.0f));
        memcpy(dst.data + index_h * dst_w * 3, tmp.data, tmp_w * tmp_h * 3);
        effect_area.x_ = 0;
        effect_area.y_ = index_h;
        effect_area.width_ = tmp_w;
        effect_area.height_ = tmp_h;
    }
    else {
        printf("error\n");
    }

    return 0;
}

void scale_bbox(int src_w, int src_h, std::vector<box_info> &bboxes, const object_rect &effect_roi)
{
    float const width_ratio = static_cast<float>(src_w) / static_cast<float>(effect_roi.width_);
    float const height_ratio = static_cast<float>(src_h) / static_cast<float>(effect_roi.height_);
    for(auto &bbox : bboxes){
        bbox.rect_.x = (bbox.rect_.x - effect_roi.x_) * width_ratio;
        bbox.rect_.y = (bbox.rect_.y - effect_roi.y_) * height_ratio;
        bbox.x2_ = (bbox.x2_ - effect_roi.x_) * width_ratio;
        bbox.y2_ = (bbox.y2_ - effect_roi.y_) * height_ratio;
    }
}

}
