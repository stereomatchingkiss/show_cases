#pragma once

#include "Rect.hpp"
#include "KalmanFilter.hpp"

#include <cstddef>

namespace flt::cvt::tracker
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
    STrack(Rect<float> const &rect, float score, int label);
    ~STrack();

    Rect<float> const &getRect() const noexcept;
    STrackState getSTrackState() const noexcept;

    bool isActivated() const noexcept;
    int getLabel() const noexcept;
    float getScore() const noexcept;
    size_t getTrackId() const noexcept;
    size_t getFrameId() const noexcept;
    size_t getStartFrameId() const noexcept;
    size_t getTrackletLength() const noexcept;

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
    int label_;
    float score_;
    size_t track_id_;
    size_t frame_id_;
    size_t start_frame_id_;
    size_t tracklet_len_;

    void updateRect();
};

}
