#include "core/BasicBehaviors.hpp"

using namespace core::behavior::basic;

const Point &Position::position() const
{
    return _position;
}

void Position::set_position(const Point& position)
{
    _position = position;
}
uint32_t Z_Order::z_order() const
{
    return _z_order;
}

void Z_Order::set_z_order(uint32_t z_order)
{
    _z_order = z_order;
}

void Changed::set_changed()
{
    _changed = true;
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