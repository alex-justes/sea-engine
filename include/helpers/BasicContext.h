#ifndef ENGINE_BASICCONTEXT_H
#define ENGINE_BASICCONTEXT_H

#include "core/Types.h"
#include "core/Context.h"
#include "core/CollisionDetectors.hpp"
#include "core/BasicActors.h"
#include "core/Camera.h"

namespace helpers::context
{
    class ObjectManager;
    class WorldManager;



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
        using Collisions = std::list<const Object*>;
        void set_collision_size(const Size &size);
        const Size &collision_size() const;
        virtual ~CollidableObject() = default;
        Collisions& collisions();
        virtual bool update(bool force) override;
    private:
        Size _collision_size{0, 0};
        Collisions _collisions;
    };

    class RenderableObject :
            public virtual Object,
            public virtual UpdatableObject,
            public core::behavior::Renderable
    {
    public:
        virtual bool update(bool force=false) override;
        virtual ~RenderableObject() = default;
    };

    class GameObject :
            public CollidableObject,
            public RenderableObject,
            public core::actor::Actor
    {
    public:
        void set_position(const Point &pos) override;
        virtual bool update(bool force) override;
        virtual bool act(uint32_t time_elapsed) override;
        virtual ~GameObject() = default;
    };

    class WorldManager;

    class ObjectManager
    {
        friend class WorldManager;
    private:
        using Item = std::unique_ptr<Object>;
        using Objects = std::map<Id, Item>;
    public:
        using const_iterator = typename Objects::const_iterator;
        using iterator = typename Objects::iterator;
        Object *get(Id id);
        const_iterator cbegin() const;
        const_iterator cend() const;
        iterator begin();
        iterator end();
        virtual ~ObjectManager() = default;
    protected:
        ObjectManager() = default;
        template<class T, class ... Types>
        T *create(Types &&... args);
        void remove(Id id);
        void remove(Object *object);
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
        template<class T, class ... Types>
        T *create_object(Types &&... args);
        Object *get_object(Id id);
        void remove_object(Id id);
        void remove_object(Object *object);
        void update_objects();
        void update_cameras();
        core::Camera *create_camera(const Point &position, const Size &size);
        void remove_camera(core::Camera *camera);
        void remove_camera(Id id);
        Collisions check_collisions();
        ObjectManager& object_manager();
        void set_time_elapsed(uint32_t time_elapsed);
    protected:
        void add_object(Object *object);
        void add_to_creation_queue(Object *object);
    private:
        std::list<Object *> _objects_to_add;
        CollisionDetector _collision_detector;
        RenderDetector _render_detector;
        core::CameraManager _camera_manager;
        core::ScreenManager &_screen_manager;
        ObjectManager _object_manager;
        Size _world_size{0, 0};
        uint32_t _time_elapsed;
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
        WorldManager &world_manager();
    private:
        WorldManager _world_manager;
        ContextManager _context_manager;
    };

    // ===============================================================================================
    template<class T, class... Types>
    T *ObjectManager::create(Types &&... args)
    {
        static_assert(std::is_base_of_v<Object, T>, "T should be derived from Object");
        auto ptr = new T(std::forward<Types>(args)...);
        ptr->_object_manager = this;
        auto item = Item(ptr);
        _objects.emplace(item->unique_id(), std::move(item));
        return ptr;
    }

    template<class T, class... Types>
    T *WorldManager::create_object(Types &&... args)
    {
        auto ptr = _object_manager.create<T>(std::forward<Types>(args)...);
        add_to_creation_queue(ptr);
        return ptr;
    }
}

#endif //ENGINE_BASICCONTEXT_H
