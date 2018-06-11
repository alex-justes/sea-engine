#ifndef ENGINE_BASICOBJECTS_H
#define ENGINE_BASICOBJECTS_H

#include "core/BasicBehaviors.hpp"
#include "core/ComplexBehaviors.hpp"
#include "core/BasicActors.h"

namespace core::object::basic
{
    using namespace core;
    class ObjectManager;

    class Object :
            public virtual behavior::basic::UniqueId<Id>,
            public virtual behavior::basic::Dead
    {
    public:
        virtual ~Object() = default;
    protected:
        Object() = default;
        ObjectManager* object_manager();
        void set_object_manager(ObjectManager* manager);
    private:
        ObjectManager* _object_manager {nullptr};
    };

    class InitializableObject :
            public virtual Object,
            public actor::basic::Initialize
    { };

    class UpdatableObject :
            public virtual Object,
            public virtual behavior::basic::Changed,
            public actor::basic::Update
    { };

    class CollidableObject :
            public virtual InitializableObject,
            public virtual behavior::basic::Position,
            public virtual behavior::basic::CollisionShape<AABB>,
            public virtual behavior::basic::CollisionSize<Size>,
    {
        friend class ObjectManager;
    public:
        using Collisions = std::list<const CollidableObject*>;
        const Collisions& collisions() const;
    private:
        Collisions& get_collisions();
    private:
        Collisions _collisions;
    };

    class RenderableObject:
            public virtual InitializableObject,
            public virtual behavior::complex::Renderable
    { };


}

#endif //ENGINE_BASICOBJECTS_H
