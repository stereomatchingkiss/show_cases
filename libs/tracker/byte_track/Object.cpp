#include "Object.hpp"

byte_track::Object::Object(Rect<float> const& _rect,
                           int _label,
                           float _prob) : rect(_rect), label(_label), prob(_prob)
{
}
