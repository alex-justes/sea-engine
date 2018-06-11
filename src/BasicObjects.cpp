#include "core/BasicObjects.h"

using namespace core::basic::object;

helpers::context::WorldManager* Object::world_manager()
{
    return _world_manager;
}

void Object::set_world_manager(helpers::context::WorldManager *manager)
{
    _world_manager = manager;
}

const CollidableObject::Collisions& CollidableObject::collisions() const
{
    return _collisions;
}

void CollidableObject::clear_collisions()
{
    _collisions.clear();
}

CollidableObject::Collisions& CollidableObject::get_collisions()
{
    return _collisions;
}

