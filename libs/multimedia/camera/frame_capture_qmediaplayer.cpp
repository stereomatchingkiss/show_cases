#include "frame_capture_qmediaplayer.hpp"

#include "frame_process_controller.hpp"
#include "frame_capture_qmediaplayer_params.hpp"

#include <QAudioOutput>
#include <QMediaMetaData>
#include <QMediaPlayer>
#include <QVideoFrame>
#include <QVideoSink>

namespace flt::mm{

struct frame_capture_qmediaplayer::impl : public QObject
{


public:
    impl(frame_capture_qmediaplayer_params const &params) :
        params_{params}
    {
        if(params.audio_on_){
            player_.setAudioOutput(&output_);
        }

        if(params_.loop_infinite_){
            player_.setLoops(QMediaPlayer::Infinite);
        }

        player_.setSource(QUrl(params_.url_));
        player_.setVideoOutput(&sink_);

        set_fps();
    }

    void remove(void *key)
    {
        auto func = [key](auto const &val)
        {
            return key == val.second;
        };
        if(auto it = std::ranges::find_if(controllers_, func); it != std::end(controllers_)){
            controllers_.erase(it);
        }
    }

    void set_fps()
    {
        if(QList<QMediaMetaData> const data = player_.videoTracks(); !data.empty()){
            int const frame_rate = data[0].value(QMediaMetaData::VideoFrameRate).toInt();
            qreal const ratio = 1.0/frame_rate;
            player_.setPlaybackRate(params_.max_fps_ * ratio);
            qDebug()<<"player play back rate == "<<player_.playbackRate();
        }
    }

    std::vector<std::pair<std::shared_ptr<frame_process_controller>, void*>> controllers_;
    QAudioOutput output_;
    frame_capture_qmediaplayer_params params_;
    QMediaPlayer player_;
    QVideoSink sink_;
};

frame_capture_qmediaplayer::frame_capture_qmediaplayer(frame_capture_qmediaplayer_params const &params, QObject *parent) :
    impl_{std::make_unique<impl>(params)}
{
    connect(&impl_->player_, &QMediaPlayer::durationChanged, this, &frame_capture_qmediaplayer::duration_changed);
    connect(&impl_->player_, &QMediaPlayer::tracksChanged, this, &frame_capture_qmediaplayer::tracks_changed);
    connect(&impl_->sink_, &QVideoSink::videoFrameChanged, this, &frame_capture_qmediaplayer::video_frame_changed);
}

frame_capture_qmediaplayer::~frame_capture_qmediaplayer()
{

}

void frame_capture_qmediaplayer::add_listener(std::shared_ptr<frame_process_controller> process_controller, void *key)
{
    impl_->controllers_.emplace_back(process_controller, key);
}

void frame_capture_qmediaplayer::remove_listener(void *key)
{
    impl_->remove(key);
}

void frame_capture_qmediaplayer::start()
{
    impl_->player_.play();
}

bool frame_capture_qmediaplayer::is_seekable() const noexcept
{
    return impl_->player_.isSeekable();
}

int frame_capture_qmediaplayer::position() const noexcept
{
    return impl_->player_.position();
}

void frame_capture_qmediaplayer::set_position(qint64 position)
{
    impl_->player_.setPosition(position);
}

void frame_capture_qmediaplayer::tracks_changed()
{
    impl_->set_fps();
}

void frame_capture_qmediaplayer::video_frame_changed(QVideoFrame const &frame)
{
    if(auto mat = frame.toImage(); !mat.isNull()){
        for(auto &val : impl_->controllers_){
            emit val.first->process_results(mat);
        }
    }else{
        qDebug()<<__func__<<":cannot decode message";
    }
}

}
