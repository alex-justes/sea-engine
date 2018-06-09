#include "core/Drawable.h"

using namespace core::drawable;


std::list<const Drawable *> CompoundDrawable::get_drawables() const
{
    return std::list<const Drawable *>();
}

const Rect::Roi& Rect::roi() const
{
    return _roi;
}

const RGBA& Rect::color() const
{
    return _color;
}