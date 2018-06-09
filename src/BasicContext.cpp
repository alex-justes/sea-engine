#include "helpers/BasicContext.h"
#include "core/Events.h"

using namespace helpers::context;

void ObjectManager::remove(Id id)
{
    _objects.erase(id);
}

void ObjectManager::remove(Object *object)
{
    if (object != nullptr)
    {
        remove(object->unique_id());
    }
}

Object* ObjectManager::get(Id id)
{
    auto item = _objects.find(id);
    if (item == _objects.end())
    {
        return nullptr;
    }
    return item->second.get();
}

ObjectManager::const_iterator ObjectManager::cbegin() const
{
    return _objects.cbegin();
}

ObjectManager::const_iterator ObjectManager::cend() const
{
    return _objects.cend();
}

void WorldManager::add_object(helpers::context::Object *object)
{
    if (object == nullptr)
    {
        return;
    }
    if (_objects.count(object->unique_id()) > 0)
    {
        return;
    }

    _objects[object->unique_id()] = object;
    LOG_D("Added %d to the world.", object->unique_id())
    auto collidable = dynamic_cast<CollidableObject*>(object);
    if (collidable != nullptr)
    {
        _collision_detector.add(*collidable);
        LOG_D("Added %d to collision_detector.", collidable->unique_id())
    }
    auto renderable = dynamic_cast<RenderableObject*>(object);
    if (renderable != nullptr)
    {
        _render_detector.add(*renderable);
        LOG_D("Added %d to render_detector.", renderable->unique_id())
    }
}

void WorldManager::remove_object(Id id)
{
    if (_objects.count(id) == 0)
    {
        return;
    }
    _collision_detector.remove(id);
    _render_detector.remove(id);
    _objects.erase(id);
}

void WorldManager::remove_object(helpers::context::Object *object)
{
    if (object == nullptr)
    {
        return;
    }
    remove_object(object->unique_id());
}

BasicContext::BasicContext(core::EventManager &event_manager, core::ScreenManager &screen_manager)
        :
        Context(event_manager, screen_manager)
{

}

ObjectManager& BasicContext::object_manager()
{
    return _object_manager;
}

WorldManager& BasicContext::world_manager()
{
    return _world_manager;
}

void BasicContext::evaluate()
{
    Item event;
    while (events_pop(event))
    {
        LOG_S("Event!")
    }
}

void BasicContext::initialize()
{
    subscribe(core::EventType::Mouse);
    subscribe(core::EventType::Keyboard);

    auto object = object_manager().create<GameObject>();
    auto shape = object->set_drawable<core::drawable::DrawableRect>();
    shape->size() = Size {100, 50};

    auto id2 = object_manager().create<GameObject>();
    auto id3 = object_manager().create<Object>();
    world_manager().add_object(object);
    world_manager().add_object(id2);
    world_manager().add_object(id3);
//    object_manager().remove(id2);
//    id2 = object_manager().create<GameObject>();
}


void GameObject::set_collision_size(const Size &size)
{
    _changed = true;
    _collision_size = size;
}

void GameObject::set_position(const Point &pos)
{
    _changed = true;
    position() = pos;
}

bool GameObject::update()
{
    if (_changed)
    {
        collision_shape() = AABB(position(), position() + _collision_size);

        auto single = dynamic_cast<core::drawable::SingleDrawable*>(this);
        auto compound = dynamic_cast<core::drawable::CompoundDrawable*>(this);
        if (single != nullptr)
        {
            auto rect = dynamic_cast<core::drawable::DrawableRect*>(this);
            if (rect != nullptr)
            {
                render_shape() = AABB(position(), position() + rect->size());
            }
        }
        else if (compound != nullptr)
        {
            // TODO: implement
        }
        _changed = false;
        return true;
    }
    return false;
}

