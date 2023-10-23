#pragma once

#include "Rect.hpp"

namespace byte_track
{

struct Object
{
    Rect<float> rect;
    int label;
    float prob;

    Object(Rect<float> const &_rect,
           int const &_label,
           float const &_prob);
};

}
