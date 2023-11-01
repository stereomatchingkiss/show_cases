#pragma once

namespace flt::tracker{

struct track_results
{
    size_t count_top_pass_ = 0;
    size_t count_bottom_pass_ = 0;
    size_t count_left_pass_ = 0;
    size_t count_right_pass_ = 0;
    size_t count_in_center_ = 0;
};

}
