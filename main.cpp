#include <iostream>
#include <filesystem>

#include "core/Engine.h"

#include "core/BasicBehaviors.hpp"
#include "core/CollisionDetectors.hpp"
#include "Log.h"
#include "helpers/Configuration.h"


/*using AABB = helpers::containers::AABB;

class Object : virtual public core::behavior::UniqueId<uint32_t>
{
public:
    using unique_id_type = core::behavior::UniqueId<uint32_t>::unique_id_type;
    virtual ~Object() = default;
};

class Collidable : public core::behavior::CollisionShape<AABB>,
                   virtual public Object
{
};

class Renderable : public core::behavior::RenderShape<AABB>,
                   virtual public Object
{
};

class GameObject :
        public Collidable,
        public Renderable
{
public:
    using unique_id_type = typename Object::unique_id_type;
    GameObject(const AABB &collision_shape, const AABB &render_shape)
    {
        this->collision_shape() = collision_shape;
        this->render_shape() = render_shape;
    }
};


class ObjectManager
{
public:
    using Id = Object::unique_id_type;
    using BroadCollisionDetector = core::collision_detector::BroadAABBCollisionDetector<Collidable,
            core::behavior::CollisionShape>;
    using RenderDetector = core::collision_detector::BroadAABBCollisionDetector<Renderable,
            core::behavior::RenderShape>;

    ObjectManager() = delete;
    ObjectManager(BroadCollisionDetector *broad_collision_detector,
                  RenderDetector *render_detector);
    template<class T, class ... Types>
    T *create(Types &&... args);
    void remove(Id id);
    Object *get(Id id) const;
protected:
private:
    using ObjectPtr = std::unique_ptr<Object>;
    using ObjectMap = std::map<Id, ObjectPtr>;
    ObjectMap _objects;
    BroadCollisionDetector *_broad_collision_detector{nullptr};
    RenderDetector *_render_detector{nullptr};
};

ObjectManager::ObjectManager(BroadCollisionDetector *broad_collision_detector,
                             RenderDetector *render_detector)
        :
        _broad_collision_detector(broad_collision_detector),
        _render_detector(render_detector)
{

}

template<class T, class ... Types>
T *ObjectManager::create(Types &&... args)
{
    static_assert(std::is_base_of_v<Object, T>, "T should be derived from Object");
    auto object_ptr = new T(std::forward<Types>(args)...);
    ObjectPtr object{static_cast<Object *>(object_ptr)};
    _objects.emplace(object_ptr->unique_id(), std::move(object));
    auto renderable_object = dynamic_cast<Renderable *>(object_ptr);
    if (renderable_object != nullptr)
    {
        _render_detector->add(*renderable_object);
    }
    auto collidable_object = dynamic_cast<Collidable *>(object_ptr);
    if (collidable_object != nullptr)
    {
        _broad_collision_detector->add(*collidable_object);
    }
    return object_ptr;
}

void ObjectManager::remove(ObjectManager::Id id)
{
    _objects.erase(id);
}

Object *ObjectManager::get(ObjectManager::Id id) const
{
    auto item = _objects.find(id);
    if (item == _objects.end())
    {
        return nullptr;
    }
    return item->second.get();
}

class BasicContext
{
public:
    using BroadCollisionDetector = core::collision_detector::HierarchicalSpatialGrid<Collidable, core::behavior::CollisionShape>;
    using RenderDetector = core::collision_detector::HierarchicalSpatialGrid<Renderable, core::behavior::RenderShape>;
    using Size = helpers::containers::Point2D<uint32_t>;
    BasicContext(const Size &world_size);
    virtual ~BasicContext() = default;
public:
    ObjectManager &object_manager();
    const ObjectManager &object_manager() const;
private:
    ObjectManager _object_manager;
    BroadCollisionDetector _broad_collision_detector;
    RenderDetector _render_detector;
};

BasicContext::BasicContext(const Size &world_size)
        :
        _broad_collision_detector(world_size),
        _render_detector(world_size),
        _object_manager(&_broad_collision_detector, &_render_detector)
{

}

ObjectManager &BasicContext::object_manager()
{
    return _object_manager;
}

const ObjectManager &BasicContext::object_manager() const
{
    return _object_manager;
}*/



int main(int argc, char **argv)
{
    namespace fs = std::filesystem;
    fs::path config_path = "engine.conf";
    if (argc < 2)
    {
        LOG_W("No configuration specified. Default \"%s\" will be used.", config_path.c_str())
    } else if (argc > 1)
    {
        config_path = fs::path(argv[1]);
    }

    LOG_S("Using \"%s\" for engine configuration.", config_path.c_str())

    if (!fs::exists(config_path))
    {
        LOG_E("Configuration \"%s\" not found.", config_path.c_str())
        LOG_S("Finished.")
        return 1;
    }

    core::Engine engine(config_path);
    if (!engine.initialize_sdl())
    {
        return 1;
    }

    engine.main_loop();

    return 0;
};