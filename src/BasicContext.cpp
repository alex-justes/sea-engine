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

Object *ObjectManager::get(Id id)
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

void WorldManager::set_world_size(const Size &size)
{
    _world_size = size;
    _collision_detector.set_world_size(size);
    _render_detector.set_world_size(size);
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
    auto collidable = dynamic_cast<CollidableObject *>(object);
    if (collidable != nullptr)
    {
        _collision_detector.add(*collidable);
        LOG_D("Added %d to collision_detector.", collidable->unique_id())
    }
    auto renderable = dynamic_cast<RenderableObject *>(object);
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

void WorldManager::update()
{
    for (auto &item: _objects)
    {
        auto &object = item.second;
        bool changed_in_action = false;
        auto actor = dynamic_cast<core::actor::Actor *>(object);
        if (actor != nullptr)
        {
            //LOG_D("Act: %d", object->unique_id())
            changed_in_action = actor->act();
        }
        bool updated = false;
        auto updatable = dynamic_cast<core::behavior::Updatable *>(object);
        if (updatable != nullptr)
        {
           // LOG_D("Update: %d", object->unique_id())
            updated = updatable->update();
        }
        if (changed_in_action || updated)
        {
           // LOG_D("Update collision detectors for %d", object->unique_id())
            _collision_detector.update(object->unique_id());
            _render_detector.update(object->unique_id());
        }
    }
}

BasicContext::BasicContext(core::EventManager &event_manager, core::ScreenManager &screen_manager)
        :
        Context(event_manager, screen_manager)
{

}

ObjectManager &BasicContext::object_manager()
{
    return _object_manager;
}

WorldManager &BasicContext::world_manager()
{
    return _world_manager;
}

void BasicContext::evaluate()
{
    Item event;
    // Process events
    while (events_pop(event))
    {
        process_event(event.get());
    }
    LOG_D("tick...")

    // TODO: check for collisions

    // Act and update collision detectors
    world_manager().update();

    // Evaluate all available contexts
    for (auto &item: _context_manager)
    {
        auto &context = item.second;
        if (!context->paused())
        {
            context->evaluate();
        }
    }

    // TODO: update cameras
}

void BasicContext::process_event(const core::Event *event)
{
}

void BasicContext::initialize()
{
    set_finished(false);
    subscribe(core::EventType::Mouse);
    subscribe(core::EventType::Keyboard);

    world_manager().set_world_size(Size(100,100));

    auto object = object_manager().create<GameObject>();
    auto shape = object->set_drawable<core::drawable::DrawableRect>();
    shape->size() = Size{100, 50};

    auto id2 = object_manager().create<GameObject>();
    auto id3 = object_manager().create<Object>();
    world_manager().add_object(object);
    world_manager().add_object(id2);
    world_manager().add_object(id3);
//    object_manager().remove(id2);
//    id2 = object_manager().create<GameObject>();
    world_manager().update();
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
        if (drawable() != nullptr)
        {
            render_shape() = AABB(drawable()->bounding_box().top_left + position(),
                                  drawable()->bounding_box().bottom_right + position());
        }
        _changed = false;
        return true;
    }
    return false;
}

bool GameObject::act()
{
    return false;
}

core::Context *ContextManager::create_context(const char *obj_file,
                                              core::EventManager &event_manager,
                                              core::ScreenManager &screen_manager,
                                              bool initialize)
{
    auto context = _context_loader.load_context(obj_file, event_manager, screen_manager);
    if (context == nullptr)
    {
        return nullptr;
    }
    if (initialize)
    {
        context->initialize();
    }
    _map.emplace(context->unique_id(), context);
    return context;
}

void ContextManager::remove_context(core::Context *context)
{
    if (context != nullptr)
    {
        remove_context(context->unique_id());
    }
}

void ContextManager::remove_context(Id id)
{
    if (_map.count(id) == 0)
    {
        return;
    }
    _context_loader.unload_context(id);
    _map.erase(id);
}

ContextManager::iterator ContextManager::begin()
{
    return _map.begin();
}

ContextManager::iterator ContextManager::end()
{
    return _map.end();
}

ContextManager::const_iterator ContextManager::cbegin() const
{
    return _map.cbegin();
}

ContextManager::const_iterator ContextManager::cend() const
{
    return _map.cend();
}