#pragma once

namespace flt::mm{

enum class stream_receiver_type : int
{
   opencv,
   websocket
};

enum class stream_source_type : int
{
    webcam,
    video,
    rtsp,
    websocket,
    hls
};

}
