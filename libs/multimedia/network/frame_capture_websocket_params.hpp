#pragma once

#include <QUrl>

namespace flt::mm{

struct frame_capture_websocket_params
{
    size_t max_fps_ = 0;
    QUrl url_;
};

}
