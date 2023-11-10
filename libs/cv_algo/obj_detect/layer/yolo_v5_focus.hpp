#pragma once

#include <layer.h>

namespace flt::cvt::det{

// YOLOX use the same focus in yolov5
class yolo_v5_focus : public ncnn::Layer
{
public:
    yolo_v5_focus();

    int forward(const ncnn::Mat &bottom_blob, ncnn::Mat &top_blob, const ncnn::Option &opt) const override;
};

inline ::ncnn::Layer* yolov5focus_layer_creater(void*)
{
    return new yolo_v5_focus;
}

}
