#ifndef ENGINE_BASICCONTEXT_H
#define ENGINE_BASICCONTEXT_H

#include "core/Context.h"
#include "core/CollisionDetectors.hpp"

namespace helpers::context
{
    using Id = uint32_t;
    using AABB = helpers::containers::AABB;
    using Roi = helpers::containers::Rect2D<uint32_t>;

    class Object : public core::behavior::UniqueId<Id>
    {
    };

    class CollidableObject :
            public virtual Object,
            public core::behavior::CollisionShape<AABB>
    {

    };

    class RenderableObject :
            public virtual Object,
            public core::behavior::Renderable
    {

    };

    class GameObject :
            public CollidableObject,
            public RenderableObject
    {
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
    public:
        virtual ~WorldManager() = default;
        void add_object(Object *object);
        // void remove_object(Id id);
        // void remove_object(Object *object);
    private:
        using Item = Object *;
        using Objects = std::map<Id, Item>;
        template<class T, template<class> class Behavior>
        using BroadCollisionDetector = typename core::collision_detector::HierarchicalSpatialGrid<T, Behavior>;
        using CollisionDetector = BroadCollisionDetector<CollidableObject, core::behavior::CollisionShape>;
        using RenderDetector = BroadCollisionDetector<RenderableObject, core::behavior::RenderShape>;
        Objects _objects;
        CollisionDetector _collision_detector;
        RenderDetector _render_detector;
    };

    class BasicContext : public core::Context
    {
    public:
        BasicContext() = delete;
        BasicContext(core::EventManager &event_manager, core::ScreenManager &screen_manager);
        virtual void evaluate() override;
        virtual void initialize() override;
    protected:
        ObjectManager &object_manager();
        WorldManager &world_manager();
    private:
        ObjectManager _object_manager;
        WorldManager _world_manager;
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
