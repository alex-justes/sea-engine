#include "core/BasicBehaviors.hpp"

using namespace core::behavior;

const Point &Position::position() const
{
    return _position;
}
Point &Position::position()
{
    return _position;
}
uint32_t Renderable::z_order() const
{
    return _z_order;
}

const Renderable::Drawable *Renderable::drawable() const
{
    return _drawable.get();
}

Renderable::Drawable *Renderable::drawable()
{
    return _drawable.get();
}
