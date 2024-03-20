#pragma once

#include <QUrl>

namespace flt::mm{

struct frame_capture_websocket_params
{
    bool is_json_ = false; //if true, that mean the data received is a json file rather than encoded image
    size_t max_fps_ = 0;
    QUrl url_;
};

}
