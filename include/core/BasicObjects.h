#ifndef ENGINE_BASICOBJECTS_H
#define ENGINE_BASICOBJECTS_H

#include "core/BasicBehaviors.hpp"
#include "core/ComplexBehaviors.hpp"
#include "core/BasicActors.h"
#include "core/Types.h"

namespace core::basic::object
{
    using namespace core;

    class Object :
            public virtual basic::behavior::UniqueId<Id>,
            public virtual basic::behavior::Dead
    {
        friend class helpers::context::WorldManager;
    public:
        virtual ~Object() = default;
    protected:
        Object() = default;
        helpers::context::WorldManager *world_manager();
    private:
        void set_world_manager(helpers::context::WorldManager *manager);
        helpers::context::WorldManager *_world_manager{nullptr};
    };

    class InitializableObject :
            public virtual Object,
            public basic::actor::Initialize
    {
    };

    class UpdatableObject :
            public virtual Object,
            public virtual basic::behavior::Changed,
            public basic::actor::Update
    {
    };

    class CollidableObject :
            public virtual InitializableObject,
            public virtual basic::behavior::Position,
            public virtual basic::behavior::CollisionShape<AABB>,
            public virtual basic::behavior::CollisionSize<Size>
    {
        friend class helpers::context::BasicContext;

    public:
        using Collisions = std::list<const CollidableObject *>;
        const Collisions &collisions() const;
        void clear_collisions();
    private:
        Collisions &get_collisions();
    private:
        Collisions _collisions;
    };

    class RenderableObject :
            public virtual InitializableObject,
            public virtual complex::behavior::Renderable
    {
    };


}

#endif //ENGINE_BASICOBJECTS_H
