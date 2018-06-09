#include "core/Drawable.h"

using namespace core::drawable;

const Drawable* Drawable::get_drawable() const
{
    return this;
}


std::list<const Drawable *> CompoundDrawable::get_drawables() const
{
    return std::list<const Drawable *>();
}

const DrawableShape *SingleDrawable::shape() const
{
    return _shape.get();
}

const RectShape::Rect& RectShape::rect() const
{
    return _rect;
}

const RGBA& RectShape::color() const
{
    return _color;
}