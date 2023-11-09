#pragma once

#include <QString>

namespace flt::mm{

struct frame_capture_qmediaplayer_params
{
    bool audio_on_ = false;
    bool loop_infinite_ = false;
    int max_fps_ = 30;
    QString url_;
};

}
