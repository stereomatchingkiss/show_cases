#pragma once

#include <vector>

namespace flt::cvt::tracker{

struct track_duration
{
    track_duration(int id, int duration_sec) :
        id_(id),
        duration_sec_(duration_sec)
    {}

    track_duration() = default;

    int id_;
    int duration_sec_;
};

struct track_results
{
    size_t count_top_pass_ = 0;
    size_t count_bottom_pass_ = 0;
    size_t count_left_pass_ = 0;
    size_t count_right_pass_ = 0;
    size_t count_in_center_ = 0;

    std::vector<track_duration> track_durations_;
};

}
