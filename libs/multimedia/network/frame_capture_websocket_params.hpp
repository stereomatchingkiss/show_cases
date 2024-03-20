#pragma once

#include <QUrl>

namespace flt::mm{

struct frame_capture_websocket_params
{
    bool not_encoded_image_ = false; //if false, that mean the data received is not an encoded image
    size_t max_fps_ = 0;
    QUrl url_;
};

}
