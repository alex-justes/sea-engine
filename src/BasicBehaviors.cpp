#include "core/BasicBehaviors.hpp"

using namespace core::behavior;

const Point &Position::position() const
{
    return _position;
}

void Position::set_position(const Point& position)
{
    _position = position;
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
