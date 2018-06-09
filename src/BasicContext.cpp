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

Object* ObjectManager::get(helpers::context::Id id)
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
    auto id1 = object_manager().create<GameObject>();
    auto shape = id1->create_drawable<core::drawable::Rect>();
    auto id2 = object_manager().create<GameObject>();
    auto id3 = object_manager().create<Object>();
    world_manager().add_object(id1);
    world_manager().add_object(id2);
    world_manager().add_object(id3);
//    object_manager().remove(id2);
//    id2 = object_manager().create<GameObject>();
}

