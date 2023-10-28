#pragma once

#include "Rect.hpp"

namespace ocv::tracker
{

struct Object
{
    Rect<float> rect;
    int label;
    float prob;

    Object(Rect<float> const &_rect, int _label, float _prob);
};

}
