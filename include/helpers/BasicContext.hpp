#ifndef ENGINE_BASICCONTEXT_H
#define ENGINE_BASICCONTEXT_H

#include "core/Types.h"
#include "core/Context.h"
#include "core/CollisionDetectors.hpp"
#include "core/BasicObjects.h"
#include "core/Camera.h"

namespace helpers::context
{
    using namespace core;
    using Object = basic::object::Object;


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
        using CollisionDetector = BroadCollisionDetector<basic::object::CollidableObject, basic::behavior::CollisionShape>;
        using RenderDetector = BroadCollisionDetector<basic::object::RenderableObject, basic::behavior::RenderShape>;
    public:
        using Collisions = CollisionDetector::PairCollisions;
        WorldManager(core::ScreenManager &screen_manager, const context::Context* current_context);
        virtual ~WorldManager();
        void set_world_size(const Size &size);
        const Size &world_size() const;
        template<class T, class ... Types>
        T *create_object(Types &&... args);
        Object *get_object(Id id);
        void remove_object(Id id);
        void remove_object(Object *object);
        void initialize_objects();
        void evaluate_objects(uint32_t time_elapsed);
        void update_objects();
        void clear_collisions();
        void update_cameras();
        void check_dead_objects();
        void remove_objects();
        core::Camera *create_camera(const Point &position, const Size &size);
        void remove_camera(core::Camera *camera);
        void remove_camera(Id id);
        Collisions check_collisions();
        ObjectManager& object_manager();
        void set_time_elapsed(uint32_t time_elapsed);
    protected:
        WorldManager() = default;
        void add_object(Object *object);
        void remove_object_impl(Id id);
    private:
        struct UpdateInfo
        {
            basic::actor::Update* actor;
            basic::object::CollidableObject* collidable;
            basic::object::RenderableObject* renderable;
            basic::object::Object*           object;
        };
        std::list<basic::actor::Initialize  *> _initialization_list;
        std::map<Id, basic::actor::Evaluate *> _actors_to_evaluate;
        std::map<Id, UpdateInfo> _objects_to_update;
        std::set<Id> _death_note;
        CollisionDetector _collision_detector;
        RenderDetector _render_detector;
        core::CameraManager _camera_manager;
        core::ScreenManager &_screen_manager;
        ObjectManager _object_manager;
        Size _world_size{0, 0};
        uint32_t _time_elapsed;
        const context::Context* _current_context;
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
        virtual void act();
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
        auto item = Item(ptr);
        _objects.emplace(item->unique_id(), std::move(item));
        return ptr;
    }

    template<class T, class... Types>
    T *WorldManager::create_object(Types &&... args)
    {
        auto ptr = _object_manager.create<T>(std::forward<Types>(args)...);
        ptr->set_world_manager(this);
        if constexpr (std::is_base_of_v<basic::actor::Initialize, T>)
        {
            LOG_D("Added %d to initialization list", ptr->unique_id())
            _initialization_list.push_back(static_cast<basic::actor::Initialize*>(ptr));
        }
        else
        {
            add_object(ptr);
        }
        return ptr;
    }
}

#endif //ENGINE_BASICCONTEXT_H
