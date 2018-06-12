#include "helpers/BasicContext.hpp"
#include "core/Events.h"

using namespace helpers::context;
using namespace core;

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
ObjectManager::iterator ObjectManager::begin()
{
    return _objects.begin();
}

ObjectManager::iterator ObjectManager::end()
{
    return _objects.end();
}

WorldManager::WorldManager(core::ScreenManager &screen_manager, const context::Context* current_context)
        :
        _screen_manager(screen_manager),
        _current_context(current_context)
{
    _camera_manager.set_current_context(_current_context);
}

WorldManager::~WorldManager()
{
    for (auto &item: _camera_manager)
    {
        core::Camera *camera = item.second.get();
        _screen_manager.detach_camera(camera);
    }
}

Object *WorldManager::get_object(Id id)
{
    return _object_manager.get(id);
}

void WorldManager::set_world_size(const Size &size)
{
    _world_size = size;
    _collision_detector.set_world_size(size);
    _render_detector.set_world_size(size);
}

const Size &WorldManager::world_size() const
{
    return _world_size;
}

void WorldManager::add_object(Object *object)
{
    LOG_D("Added %d to the world.", object->unique_id())

    auto collidable = dynamic_cast<basic::object::CollidableObject *>(object);
    if (collidable != nullptr)
    {
        _collision_detector.add(*collidable);
        LOG_D("Added %d to collision_detector.", collidable->unique_id())
    }

    auto renderable = dynamic_cast<basic::object::RenderableObject *>(object);
    if (renderable != nullptr)
    {
        _render_detector.add(*renderable);
        LOG_D("Added %d to render_detector.", renderable->unique_id())
    }

    auto evaluatable = dynamic_cast<basic::actor::Evaluate *>(object);
    if (evaluatable != nullptr)
    {
        _actors_to_evaluate[object->unique_id()] = evaluatable;
        LOG_D("Added %d to evaluate-list.", renderable->unique_id())
    }

    auto updatable = dynamic_cast<basic::actor::Update *>(object);
    if (updatable != nullptr)
    {
        _objects_to_update[object->unique_id()] = {updatable, collidable, renderable, object};
        LOG_D("Added %d to update-list.", renderable->unique_id())
    }
}

void WorldManager::remove_object(Id id)
{
    _death_note.insert(id);
    LOG_D("Object %d added to death note.", id)
}

void WorldManager::remove_object(helpers::context::Object *object)
{
    if (object == nullptr)
    {
        return;
    }
    remove_object(object->unique_id());
}

void WorldManager::remove_objects()
{
    if (_death_note.empty())
    {
        return;
    }
    for (auto id: _death_note)
    {
        remove_object_impl(id);
    }
    _death_note.clear();
}

void WorldManager::remove_object_impl(Id id)
{
    _actors_to_evaluate.erase(id);
    _objects_to_update.erase(id);
    _collision_detector.remove(id);
    _render_detector.remove(id);
    _object_manager.remove(id);
}

void WorldManager::set_time_elapsed(uint32_t time_elapsed)
{
    _time_elapsed = time_elapsed;
}

void WorldManager::evaluate_objects(uint32_t time_elapsed)
{
    for (auto&[id, actor]: _actors_to_evaluate)
    {
        actor->evaluate(time_elapsed);
    }
}

void WorldManager::update_objects()
{
    if (_objects_to_update.empty())
    {
        return;
    }
    struct DetectorUpdateInfo
    {
        basic::object::CollidableObject *collidable;
        basic::object::RenderableObject *renderable;
        basic::object::Object *object;
    };
    std::list<DetectorUpdateInfo> detector_updates;
    for (auto&[id, update_info]: _objects_to_update)
    {
        auto&[actor, collidable, renderable, object] = update_info;
        if (actor->update(false) && (collidable != nullptr || renderable != nullptr))
        {
            detector_updates.push_back({collidable, renderable, object});
        }
    }

    for (auto &[collidable, renderable, object]: detector_updates)
    {
        auto object_with_pos = dynamic_cast<basic::behavior::Position *>(object);
        if (object_with_pos != nullptr)
        {
            auto &pos = object_with_pos->position();
            if (pos.x >= _world_size.x || pos.y >= _world_size.y)
            {
                LOG_D("Mr. Anderson tried to escape matrix (unsuccessfully)...")
                object->set_dead();
                continue;
            }
        }
        if (collidable != nullptr)
        {
            _collision_detector.update(collidable->unique_id());
        }
        if (renderable != nullptr)
        {
            _render_detector.update(renderable->unique_id());
        }
    }
}

void WorldManager::check_dead_objects()
{
    for (auto& [id, object]: _object_manager)
    {
        if (object->dead())
        {
            remove_object(object->unique_id());
        }
    }
}

void WorldManager::initialize_objects()
{
    if (_initialization_list.empty())
    {
        return;
    }
    for (auto &object: _initialization_list)
    {
        object->initialize();
        add_object(dynamic_cast<Object *>(object));
    }
    _initialization_list.clear();
}

WorldManager &BasicContext::world_manager()
{
    return _world_manager;
}

WorldManager::Collisions WorldManager::check_collisions()
{
    return _collision_detector.broad_check();
}

ObjectManager &WorldManager::object_manager()
{
    return _object_manager;
}

core::Camera *WorldManager::create_camera(const Point &position, const Size &size)
{
    Size fixed_size{
            std::min(size.x, _world_size.x),
            std::min(size.y, _world_size.y)
    };
    Point fixed_position{
            std::min(position.x, _world_size.x),
            std::min(position.y, _world_size.y)
    };
    return _camera_manager.create_camera(fixed_position, fixed_size);
}
void WorldManager::remove_camera(Id id)
{
    auto camera = _camera_manager.get_camera(id);
    remove_camera(camera);
}

void WorldManager::remove_camera(core::Camera *camera)
{
    if (camera == nullptr)
    {
        return;
    }
    _screen_manager.detach_camera(camera);
    _camera_manager.remove_camera(camera->unique_id());
}

void WorldManager::update_cameras()
{
    for (auto &item: _camera_manager)
    {
        Camera *camera = item.second.get();
        if (camera->active())
        {
            Point cam_pos = camera->position();
            Point cam_size = camera->size();
            Roi roi{cam_pos, cam_pos + cam_size};
            auto collisions = _render_detector.broad_check(roi);
            typename core::Camera::List list;
            for (const auto &id: collisions)
            {
                auto renderable = dynamic_cast<typename core::Camera::ObjectType>(_object_manager.get(id));
                if (renderable != nullptr)
                {
                    list.push_back(renderable);
                }
            }
            camera->update_visible_objects(std::move(list));
        }
    }
}

BasicContext::BasicContext(core::EventManager &event_manager, core::ScreenManager &screen_manager)
        :
        Context(event_manager, screen_manager),
        _world_manager(screen_manager, this)
{
}


void BasicContext::evaluate(uint32_t time_elapsed)
{
    world_manager().set_time_elapsed(time_elapsed);
    world_manager().initialize_objects();
    Item event;
    // Process events
    while (events_pop(event))
    {
        process_event(event.get());
    }
    // TODO: check copy elision
    // Check collisions
    process_collisions(world_manager().check_collisions());
    // Call evaluate
    world_manager().evaluate_objects(time_elapsed);
    // Call update and update collision detectors
    world_manager().update_objects();
    // Remove objects
    world_manager().check_dead_objects();
    world_manager().remove_objects();
    // Update cameras
    world_manager().update_cameras();
    // Evaluate all available contexts
    for (auto &item: _context_manager)
    {
        auto &context = item.second;
        if (!context->paused())
        {
            if (context->finished())
            {
                set_finished(true);
                break;
            }
            context->evaluate(time_elapsed);
        }
    }
}

void BasicContext::process_collisions(BasicContext::Collisions pairs)
{
    for (const auto&[id1, id2]: pairs)
    {
        auto obj1 = dynamic_cast<basic::object::CollidableObject *>(world_manager().get_object(id1));
        auto obj2 = dynamic_cast<basic::object::CollidableObject *>(world_manager().get_object(id2));
        if (obj1 != nullptr)
        {
            obj1->get_collisions().push_back(obj2);
        }
        if (obj2 != nullptr)
        {
            obj2->get_collisions().push_back(obj1);
        }
    }
}

void BasicContext::process_event(const core::Event *event)
{
}

void BasicContext::initialize()
{
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

