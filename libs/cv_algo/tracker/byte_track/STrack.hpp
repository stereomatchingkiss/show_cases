#pragma once

#include "Rect.hpp"
#include "KalmanFilter.hpp"

#include <cstddef>

namespace ocv::tracker
{

enum class STrackState {
    New = 0,
    Tracked = 1,
    Lost = 2,
    Removed = 3,
};

class STrack
{
public:
    STrack(Rect<float> const &rect, float score);
    ~STrack();

    Rect<float> const &getRect() const;
    STrackState getSTrackState() const;

    bool isActivated() const;
    float getScore() const;
    size_t getTrackId() const;
    size_t getFrameId() const;
    size_t getStartFrameId() const;
    size_t getTrackletLength() const;

    void activate(size_t frame_id, size_t track_id);
    void reActivate(STrack const &new_track, size_t frame_id, int new_track_id = -1);

    void predict();
    void update(STrack const &new_track, size_t frame_id);

    void markAsLost();
    void markAsRemoved();

private:
    KalmanFilter kalman_filter_;
    KalmanFilter::StateMean mean_;
    KalmanFilter::StateCov covariance_;

    Rect<float> rect_;
    STrackState state_;

    bool is_activated_;
    float score_;
    size_t track_id_;
    size_t frame_id_;
    size_t start_frame_id_;
    size_t tracklet_len_;

    void updateRect();
};

}
