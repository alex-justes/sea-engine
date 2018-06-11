#include "core/BasicObjects.h"

using namespace core::basic::object;

ObjectManager* Object::object_manager()
{
    return _object_manager;
}

void Object::set_object_manager(ObjectManager *manager)
{
    _object_manager = manager;
}

const CollidableObject::Collisions& CollidableObject::collisions() const
{
    return _collisions;
}

CollidableObject::Collisions& CollidableObject::get_collisions()
{
    return _collisions;
}

