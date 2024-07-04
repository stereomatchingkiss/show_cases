#include "frame_capture_creator.hpp"

#include "widget_source_selection.hpp"
#include "widget_stream_player.hpp"

#include <multimedia/camera/frame_capture_qcamera.hpp>
#include <multimedia/camera/frame_capture_qcamera_params.hpp>
#include <multimedia/camera/frame_capture_qmediaplayer.hpp>
#include <multimedia/camera/frame_capture_qmediaplayer_params.hpp>
//#include <multimedia/camera/frame_process_controller.hpp>
#include <multimedia/network/frame_capture_websocket.hpp>
#include <multimedia/network/frame_capture_websocket_params.hpp>
#include <multimedia/stream_enum.hpp>

#include <QTimer>

using namespace flt;
using namespace flt::mm;

frame_capture_creator::frame_capture_creator(widget_source_selection *source_selection,
                                             widget_stream_player *stream_player,
                                             QObject *parent)
    : QObject{parent},
    timer_{new QTimer(this)},
    widget_source_selection_{source_selection},
    widget_stream_player_{stream_player}
{
    timer_->setInterval(1000);
}

frame_capture_creator::~frame_capture_creator()
{

}

flt::mm::single_frame_with_multi_worker_base *frame_capture_creator::get_sfwmw() noexcept
{
    return sfwmw_.get();
}

void frame_capture_creator::reset()
{
    sfwmw_.reset();
}

void frame_capture_creator::create_frame_capture()
{
    timer_->stop();
    disconnect(timer_, &QTimer::timeout, this, &frame_capture_creator::update_position);
    if(widget_source_selection_->get_source_type() == stream_source_type::websocket){
        sfwmw_ = std::make_unique<frame_capture_websocket>(widget_source_selection_->get_frame_capture_websocket_params());
    }else if(widget_source_selection_->get_source_type() == stream_source_type::webcam){
        sfwmw_ = std::make_unique<frame_capture_qcamera>(widget_source_selection_->get_frame_capture_qcamera_params());
    }else{
        sfwmw_ = std::make_unique<frame_capture_qmediaplayer>(widget_source_selection_->get_frame_capture_qmediaplayer_params());
        auto player = static_cast<frame_capture_qmediaplayer*>(sfwmw_.get());

        connect(widget_stream_player_, &widget_stream_player::pause, player, &frame_capture_qmediaplayer::pause);
        connect(widget_stream_player_, &widget_stream_player::play, player, &frame_capture_qmediaplayer::start);
        connect(widget_stream_player_, &widget_stream_player::seek, player, &frame_capture_qmediaplayer::set_position);

        if(widget_source_selection_->get_source_type() == stream_source_type::hls ||
            widget_source_selection_->get_source_type() == stream_source_type::video){
            auto player = static_cast<frame_capture_qmediaplayer*>(sfwmw_.get());
            widget_stream_player_->set_is_seekable(player->is_seekable());
            widget_stream_player_->set_duration(player->position(), player->max_position());

            connect(timer_, &QTimer::timeout, this, &frame_capture_creator::update_position);

            timer_->start();
        }else{
            widget_stream_player_->set_is_seekable(false);
        }
    }
}

void frame_capture_creator::update_position()
{
    if(sfwmw_){
        auto player = static_cast<frame_capture_qmediaplayer*>(sfwmw_.get());
        widget_stream_player_->set_current_position(player->position());
    }
}
