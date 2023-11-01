#ifndef COMMON_OBJ_DET_TYPE_HPP
#define COMMON_OBJ_DET_TYPE_HPP

#include <opencv2/core.hpp>

#include <string>
#include <vector>

namespace flt::cvt::det{

struct object_rect;

struct box_info
{
    //float x1_;
    //float y1_;
    float x2_;
    float y2_;

    cv::Rect2f rect_;

    int label_;
    float score_;    
    int track_id_ = -1;    

    box_info();
    box_info(float x1, float y1, float x2, float y2, float score, int label);

    cv::Point2f center() const noexcept;
    cv::Point2f tl() const noexcept;
    cv::Point2f br() const noexcept;

    std::string to_string(int src_width, int src_height) const;
    cv::Rect to_cv_rect() const noexcept;
    cv::Rect to_cv_rect(int src_width, int src_height) const noexcept;

    object_rect to_obj_rect() const noexcept;
    object_rect to_obj_rect(int src_width, int src_height) const noexcept;

    /**
     * split input string to boxes. Input format should be
     * x1,y1,x2,y2,score,label......,x1,y1,x2,y2,score,label
     * @param input
     * @return
     */
    static std::vector<box_info> string_to_boxes(std::string const &input);
};

struct center_prior
{
    int x;
    int y;
    int stride;
};

struct object_rect {
    object_rect();
    object_rect(int x, int y, int width, int height);

    int x_;
    int y_;
    int width_;
    int height_;
};

}

#endif // COMMON_OBJ_DET_TYPE_HPP
