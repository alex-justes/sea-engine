#include "core/BasicBehaviors.hpp"

using namespace core::behavior;

const Position::Point &Position::position() const
{
    return _position;
}
Position::Point &Position::position()
{
    return _position;
}
uint32_t Renderable::z_order() const
{
    return _z_order;
}

