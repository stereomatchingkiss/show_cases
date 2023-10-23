#pragma once

#include "Rect.hpp"

namespace ocv::byte_track
{

struct Object
{
    Rect<float> rect;
    int label;
    float prob;

    Object(Rect<float> const &_rect, int _label, float _prob);
};

}
