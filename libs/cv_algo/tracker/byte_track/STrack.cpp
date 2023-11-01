#include "STrack.hpp"

#include <cstddef>

namespace flt::cvt::tracker{

STrack::STrack(const Rect<float>& rect, float score, int label) :
    kalman_filter_(),
    mean_(),
    covariance_(),
    rect_(rect),
    state_(STrackState::New),
    is_activated_(false),
    label_(label),
    score_(score),
    track_id_(0),
    frame_id_(0),
    start_frame_id_(0),
    tracklet_len_(0)
{
}

STrack::~STrack()
{
}

Rect<float> const& cvt::tracker::STrack::getRect() const noexcept
{
    return rect_;
}

STrackState cvt::tracker::STrack::getSTrackState() const noexcept
{
    return state_;
}

bool STrack::isActivated() const noexcept
{
    return is_activated_;
}

int STrack::getLabel() const noexcept
{
    return label_;
}

float STrack::getScore() const noexcept
{
    return score_;
}

size_t STrack::getTrackId() const noexcept
{
    return track_id_;
}

size_t STrack::getFrameId() const noexcept
{
    return frame_id_;
}

size_t STrack::getStartFrameId() const noexcept
{
    return start_frame_id_;
}

size_t STrack::getTrackletLength() const noexcept
{
    return tracklet_len_;
}

void STrack::activate(size_t frame_id, size_t track_id)
{
    kalman_filter_.initiate(mean_, covariance_, rect_.getXyah());

    updateRect();

    state_ = STrackState::Tracked;
    if (frame_id == 1)
    {
        is_activated_ = true;
    }
    track_id_ = track_id;
    frame_id_ = frame_id;
    start_frame_id_ = frame_id;
    tracklet_len_ = 0;
}

void STrack::reActivate(STrack const &new_track, size_t frame_id, int new_track_id)
{
    kalman_filter_.update(mean_, covariance_, new_track.getRect().getXyah());

    updateRect();

    state_ = STrackState::Tracked;
    is_activated_ = true;
    score_ = new_track.getScore();
    if (0 <= new_track_id)
    {
        track_id_ = new_track_id;
    }
    frame_id_ = frame_id;
    tracklet_len_ = 0;
}

void STrack::predict()
{
    if (state_ != STrackState::Tracked)
    {
        mean_[7] = 0;
    }
    kalman_filter_.predict(mean_, covariance_);
}

void STrack::update(STrack const &new_track, size_t frame_id)
{
    kalman_filter_.update(mean_, covariance_, new_track.getRect().getXyah());

    updateRect();

    state_ = STrackState::Tracked;
    is_activated_ = true;
    score_ = new_track.getScore();
    frame_id_ = frame_id;
    tracklet_len_++;
}

void STrack::markAsLost()
{
    state_ = STrackState::Lost;
}

void STrack::markAsRemoved()
{
    state_ = STrackState::Removed;
}

void STrack::updateRect()
{
    rect_.width() = mean_[2] * mean_[3];
    rect_.height() = mean_[3];
    rect_.x() = mean_[0] - rect_.width() / 2;
    rect_.y() = mean_[1] - rect_.height() / 2;
}

}
