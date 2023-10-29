#include "common_obj_det_type.hpp"

#include <boost/algorithm/string.hpp>

#include <opencv2/imgproc.hpp>

#include <algorithm>
#include <format>

namespace ocv::det{

box_info::box_info()
{

}

box_info::box_info(float x1, float y1, float x2, float y2, float score, int label) :
    //x1_{x1}, y1_{y1}, x2_{x2}, y2_{y2}, rect_{x1, y1, x2 - x1, y2 - y1},
    x2_{x2}, y2_{y2}, rect_{x1, y1, x2 - x1, y2 - y1},
    label_{label}, score_{score}, track_id_{-1}
{

}

cv::Point2f box_info::center() const noexcept
{
    return {(x2_ - rect_.x) / 2 + rect_.x, (y2_ - rect_.y) / 2 + rect_.y};
}

cv::Point2f box_info::tl() const noexcept
{
    return {rect_.x, rect_.y};
}

cv::Point2f box_info::br() const noexcept
{
    return {x2_, y2_};
}

std::string box_info::to_string(int src_width, int src_height) const
{
    return std::format("{},{},{},{},{:.3f},{},",
                       std::clamp(static_cast<int>(rect_.x), 0, src_width - 1),
                       std::clamp(static_cast<int>(rect_.y), 0, src_height - 1),
                       std::clamp(static_cast<int>(x2_), 0, src_width - 1),
                       std::clamp(static_cast<int>(y2_), 0, src_height - 1),
                       score_,
                       label_);
}

cv::Rect box_info::to_cv_rect() const noexcept
{
    return cv::Rect(static_cast<int>(rect_.x), static_cast<int>(rect_.y), static_cast<int>(x2_ - rect_.x), static_cast<int>(y2_ - rect_.y));
}

cv::Rect box_info::to_cv_rect(int src_width, int src_height) const noexcept
{
    auto const x1 = std::clamp(static_cast<int>(rect_.x), 0, src_width - 1);
    auto const y1 = std::clamp(static_cast<int>(rect_.y), 0, src_height - 1);
    auto const x2 = std::clamp(static_cast<int>(x2_), 0, src_width - 1);
    auto const y2 = std::clamp(static_cast<int>(y2_), 0, src_height - 1);
    return cv::Rect(static_cast<int>(x1), static_cast<int>(y1), static_cast<int>(x2 - x1), static_cast<int>(y2 - y1));
}

object_rect box_info::to_obj_rect() const noexcept
{
    return object_rect(static_cast<int>(rect_.x),
                       static_cast<int>(rect_.y),
                       static_cast<int>(x2_ - rect_.x),
                       static_cast<int>(y2_ - rect_.y));
}

object_rect box_info::to_obj_rect(int src_width, int src_height) const noexcept
{
    auto const x1 = std::clamp(static_cast<int>(rect_.x), 0, src_width - 1);
    auto const y1 = std::clamp(static_cast<int>(rect_.y), 0, src_height - 1);
    auto const x2 = std::clamp(static_cast<int>(x2_), 0, src_width - 1);
    auto const y2 = std::clamp(static_cast<int>(y2_), 0, src_height - 1);
    return object_rect(static_cast<int>(x1), static_cast<int>(y1), static_cast<int>(x2 - x1), static_cast<int>(y2 - y1));
}

std::vector<box_info> box_info::string_to_boxes(const std::string &input)
{
    if(input.empty()){
        return {};
    }

    std::vector<box_info> boxes_info;
    std::vector<std::string> outputs;
    boost::split(outputs, input, boost::is_any_of(","));
    if(!outputs.empty() && (outputs.size() % 6) == 0){
        for(size_t i = 0; i < outputs.size(); i += 6){
            box_info info;
            info.rect_.x = static_cast<float>(std::stoi(outputs[i]));
            info.rect_.y = static_cast<float>(std::stoi(outputs[i + 1]));
            info.x2_ = static_cast<float>(std::stoi(outputs[i + 2]));
            info.y2_ = static_cast<float>(std::stoi(outputs[i + 3]));
            info.score_ = std::stof(outputs[i + 4]);
            info.label_ = std::stoi(outputs[i + 5]);
            boxes_info.emplace_back(info);
        }
    }

    return boxes_info;
}

object_rect::object_rect()
{

}

object_rect::object_rect(int x, int y, int width, int height) :
    x_(x), y_(y), width_(width), height_(height)
{

}

}
