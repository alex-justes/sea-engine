#include "core/Drawable.h"

using namespace core::drawable;


std::list<const Drawable *> CompoundDrawable::get_drawables() const
{
    return std::list<const Drawable *>();
}

const Rect::Size& Rect::size() const
{
    return _size;
}

const RGBA& Rect::color() const
{
    return _color;
}
Rect::Size &Rect::size()
{
    return _size;
}

RGBA &Rect::color()
{
    return _color;
}
