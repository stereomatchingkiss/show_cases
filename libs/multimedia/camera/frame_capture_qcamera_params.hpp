#pragma once

#include <QCameraDevice>

namespace flt::mm{

struct frame_capture_qcamera_params
{
    QCameraDevice device_;
    int max_fps_ = 30;
};

}
