#include "Object.hpp"

namespace flt::cvt::tracker{

Object::Object(Rect<float> const& _rect,
               int _label,
               float _prob) : rect(_rect), label(_label), prob(_prob)
{
}

}
