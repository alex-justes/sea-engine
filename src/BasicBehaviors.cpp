#include "core/BasicBehaviors.hpp"

using namespace core::basic::behavior;

const Point &Position::position() const
{
    return _position;
}

void Position::set_position(const Point& position)
{
    _position = position;
}
int32_t Z_Order::z_order() const
{
    return _z_order;
}

void Z_Order::set_z_order(int32_t z_order)
{
    _z_order = z_order;
}

void Changed::set_changed(bool changed)
{
    _changed = changed;
}

bool Changed::changed() const
{
    return _changed;
}

void Dead::set_dead()
{
    _dead = true;
}

bool Dead::dead() const
{
    return _dead;
}

const Size& BoxSize::box_size() const
{
    return _size;
}

void BoxSize::set_box_size(const Size &size)
{
    _size = size;
}

const RGBA& FillColor::fill_color() const
{
    return _color;
}

void FillColor::set_fill_color(const RGBA &color)
{
    _color = color;
}

const RGBA& BorderColor::border_color() const
{
    return _color;
}

void BorderColor::set_border_color(const RGBA &color)
{
    _color = color;
}


