#ifndef ENGINE_BASICCONTEXT_H
#define ENGINE_BASICCONTEXT_H

#include "core/Types.h"
#include "core/Context.h"
#include "core/CollisionDetectors.hpp"
#include "core/BasicActors.h"
#include "core/Camera.h"

namespace helpers::context
{
    class Object : public core::behavior::UniqueId<Id>
    {
    public:
        virtual ~Object() = default;
    };

    class UpdatableObject :
            public virtual Object,
            public core::behavior::Updatable
    {
    public:
        void set_changed(bool changed);
        bool changed() const;
    private:
        bool _changed{true};
    };

    class CollidableObject :
            public virtual Object,
            public virtual UpdatableObject,
            public virtual core::behavior::Position,
            public core::behavior::CollisionShape<AABB>
    {
    public:
        void set_collision_size(const Size &size);
        const Size& collision_size() const;
        virtual ~CollidableObject() = default;
    private:
        Size _collision_size{0, 0};
    };

    class RenderableObject :
            public virtual Object,
            public virtual UpdatableObject,
            public core::behavior::Renderable
    {
    public:
        virtual ~RenderableObject() = default;
    };

    class GameObject :
            public CollidableObject,
            public RenderableObject,
            public core::actor::Actor
    {
    public:
        void set_position(const Point &pos);
        virtual bool update() override;
        virtual bool act() override;
        virtual ~GameObject() = default;
    };

    class ObjectManager
    {
    private:
        using Item = std::unique_ptr<Object>;
        using Objects = std::map<Id, Item>;
    public:
        using const_iterator = typename Objects::const_iterator;

        template<class T, class ... Types>
        T *create(Types &&... args);
        void remove(Id id);
        void remove(Object *object);
        Object *get(Id id);
        const_iterator cbegin() const;
        const_iterator cend() const;
        virtual ~ObjectManager() = default;
    private:
        Objects _objects;
    };

    class WorldManager
    {
    private:
        using Item = Object *;
        using Objects = std::map<Id, Item>;
        template<class T, template<class> class Behavior>
        using BroadCollisionDetector = typename core::collision_detector::HierarchicalSpatialGrid<T, Behavior>;
        using CollisionDetector = BroadCollisionDetector<CollidableObject, core::behavior::CollisionShape>;
        using RenderDetector = BroadCollisionDetector<RenderableObject, core::behavior::RenderShape>;
    public:
        using Collisions = CollisionDetector::PairCollisions;
        explicit WorldManager(core::ScreenManager &screen_manager);
        virtual ~WorldManager();
        void set_world_size(const Size &size);
        const Size &world_size() const;
        void add_object(Object *object);
        void remove_object(Id id);
        void remove_object(Object *object);
        void update_objects();
        void update_cameras();
        core::Camera *create_camera(const Point &position, const Size &size);
        void remove_camera(core::Camera *camera);
        void remove_camera(Id id);
        Collisions check_collisions();
    private:
        Objects _objects;
        CollisionDetector _collision_detector;
        RenderDetector _render_detector;
        core::CameraManager _camera_manager;
        core::ScreenManager &_screen_manager;
        Size _world_size{0, 0};
    };

    class ContextManager
    {
    private:
        using Item = core::Context *;
        using Map = std::map<Id, Item>;
    public:
        using iterator = typename Map::iterator;
        using const_iterator = typename Map::const_iterator;
        virtual ~ContextManager() = default;
        core::Context *create_context(const char *obj_file,
                                      core::EventManager &event_manager,
                                      core::ScreenManager &screen_manager,
                                      bool initialize = true);
        void remove_context(core::Context *context);
        void remove_context(Id id);
        iterator begin();
        iterator end();
        const_iterator cbegin() const;
        const_iterator cend() const;
    private:
        core::ContextLoader _context_loader;
        Map _map;
    };

    class BasicContext : public core::Context
    {
    public:
        using Collisions = WorldManager::Collisions;
        BasicContext() = delete;
        BasicContext(core::EventManager &event_manager, core::ScreenManager &screen_manager);
        virtual void evaluate(uint32_t time_elapsed) override;
        virtual void initialize() override;
        virtual void process_event(const core::Event *event);
        virtual void process_collisions(Collisions pairs);
    protected:
        ObjectManager &object_manager();
        WorldManager &world_manager();
    private:
        ObjectManager _object_manager;
        WorldManager _world_manager;
        ContextManager _context_manager;
    };

    // ===============================================================================================
    template<class T, class... Types>
    T *ObjectManager::create(Types &&... args)
    {
        static_assert(std::is_base_of_v<Object, T>, "T should be derived from Object");
        auto ptr = new T(std::forward<Types>(args)...);
        auto item = Item(ptr);
        _objects.emplace(item->unique_id(), std::move(item));
        return ptr;
    }
}

#endif //ENGINE_BASICCONTEXT_H
