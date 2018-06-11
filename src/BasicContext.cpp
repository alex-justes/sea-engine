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
ObjectManager::iterator ObjectManager::begin()
{
    return _objects.begin();
}

ObjectManager::iterator ObjectManager::end()
{
    return _objects.end();
}

WorldManager::WorldManager(core::ScreenManager &screen_manager)
        :
        _screen_manager(screen_manager)
{
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

void WorldManager::add_to_creation_queue(helpers::context::Object *object)
{

    LOG_D("Added %d to creation queue", object->unique_id())
    _objects_to_add.push_back(object);
}

void WorldManager::add_object(helpers::context::Object *object)
{
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
    auto object = _object_manager.get(id);
    if (object == nullptr)
    {
        return;
    }
    _collision_detector.remove(id);
    _render_detector.remove(id);
    _object_manager.remove(id);
}

void WorldManager::remove_object(helpers::context::Object *object)
{
    if (object == nullptr)
    {
        return;
    }
    remove_object(object->unique_id());
}

void WorldManager::set_time_elapsed(uint32_t time_elapsed)
{
    _time_elapsed = time_elapsed;
}

void WorldManager::update_objects()
{
    for (auto &item: _object_manager)
    {
        Object *object = item.second.get();
        if (object->dead())
        {
            continue;
        }
        bool changed_in_action = false;
        auto actor = dynamic_cast<core::actor::Actor *>(object);
        if (actor != nullptr)
        {
            //LOG_D("Act: %d", object->unique_id())
            changed_in_action = actor->act(_time_elapsed);
        }
        bool updated = false;
        auto updatable = dynamic_cast<core::behavior::Updatable *>(object);
        if (updatable != nullptr)
        {
            // LOG_D("Update: %d", object->unique_id())
            updated = updatable->update(false);
        }
        if (changed_in_action || updated)
        {
            // LOG_D("Update collision detectors for %d", object->unique_id())
            auto object_with_pos = dynamic_cast<core::behavior::Position *>(object);
            if (object_with_pos != nullptr)
            {
                auto &pos = object_with_pos->position();
                if (pos.x >= _world_size.x || pos.y >= _world_size.y)
                {
                    LOG_W("Mr. Anderson tried to escape matrix (unsuccessfully)...")
                    object->set_dead();
                    continue;
                }
            }
            _collision_detector.update(object->unique_id());
            _render_detector.update(object->unique_id());
        }

        auto collidable = dynamic_cast<CollidableObject *>(object);
        if (collidable != nullptr)
        {
            collidable->collisions().clear();
        }
    }
    for (auto &item: _object_manager)
    {
        Object* object = item.second.get();
        if (object->dead())
        {
            remove_object(object->unique_id());
        }
    }
    for (auto &object: _objects_to_add)
    {
        add_object(object);
    }
    _objects_to_add.clear();
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
        auto &camera = item.second;
        if (camera->active())
        {
            // Weirdo... %|
            // All Hail Big Flying Spaghetti Monster!
            Point cam_pos = ((const core::Camera *) (camera.get()))->position();
            Point cam_size = ((const core::Camera *) (camera.get()))->size();
            Point bottom_right = cam_pos + cam_size;
            Roi roi{
                    Point{
                            std::min(cam_pos.x, _world_size.x),
                            std::min(cam_pos.y, _world_size.y)
                    },
                    Point{
                            std::min(bottom_right.x, _world_size.x),
                            std::min(bottom_right.y, _world_size.y)
                    }
            };
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
        _world_manager(screen_manager)
{
}


void BasicContext::evaluate(uint32_t time_elapsed)
{
    world_manager().set_time_elapsed(time_elapsed);
    Item event;
    // Process events
    while (events_pop(event))
    {
        process_event(event.get());
    }

    // TODO: check copy elision
    // Check collisions
    process_collisions(world_manager().check_collisions());

    // Act and update collision detectors
    world_manager().update_objects();

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
    // Update cameras
    world_manager().update_cameras();
}

void BasicContext::process_collisions(BasicContext::Collisions pairs)
{
    for (const auto& [id1, id2]: pairs)
    {
        auto obj1 = dynamic_cast<helpers::context::CollidableObject*>(world_manager().get_object(id1));
        auto obj2 = dynamic_cast<helpers::context::CollidableObject*>(world_manager().get_object(id2));
        if (obj1 != nullptr)
        {
            obj1->collisions().push_back(obj2);
        }
        if (obj2 != nullptr)
        {
            obj2->collisions().push_back(obj1);
        }
    }
}

void BasicContext::process_event(const core::Event *event)
{
}

void BasicContext::initialize()
{
}


void CollidableObject::set_collision_size(const Size &size)
{
    set_changed(true);
    _collision_size = size;
}

CollidableObject::Collisions &CollidableObject::collisions()
{
    return _collisions;
}

const Size &CollidableObject::collision_size() const
{
    return _collision_size;
}

ObjectManager *Object::object_manager()
{
    return _object_manager;
}

bool Object::dead() const
{
    return _dead;
}

void Object::set_dead()
{
    _dead = true;
}

void UpdatableObject::set_changed(bool changed)
{
    _changed = changed;
}

bool UpdatableObject::changed() const
{
    return _changed;
}

void GameObject::set_position(const Point &pos)
{
    set_changed(true);
    core::behavior::Position::set_position(pos);
}

bool RenderableObject::update(bool force)
{
    if (force || changed())
    {
        if (drawable() != nullptr)
        {
            set_render_shape({drawable()->bounding_box().top_left + position(),
                              drawable()->bounding_box().bottom_right + position()});
        }
        set_changed(false);
        return true;
    }
    return false;
}

bool CollidableObject::update(bool force)
{
    if (force || changed())
    {
        collision_shape() = AABB(position(), position() + collision_size());
        set_changed(false);
        return true;
    }
    return false;
}

bool GameObject::update(bool force)
{
    if (force || changed())
    {
        CollidableObject::update(true);
        RenderableObject::update(true);
        return true;
    }
    return false;
}

bool GameObject::act(uint32_t time_elapsed)
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

