#include "core/Drawable.h"

using namespace core::drawable;


std::list<const Drawable *> CompoundDrawable::get_drawables() const
{
    return std::list<const Drawable *>();
}

const Size& DrawableRect::size() const
{
    return _size;
}

AABB DrawableRect::bounding_box() const
{
    return AABB(Point(0,0), _size);
}

const RGBA& DrawableRect::color() const
{
    return _color;
}
Size &DrawableRect::size()
{
    return _size;
}

RGBA &DrawableRect::color()
{
    return _color;
}
