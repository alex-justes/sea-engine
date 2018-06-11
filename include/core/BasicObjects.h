#ifndef ENGINE_BASICOBJECTS_H
#define ENGINE_BASICOBJECTS_H

#include "core/BasicBehaviors.hpp"
#include "core/ComplexBehaviors.hpp"
#include "core/BasicActors.h"

namespace core::object::basic
{
    using namespace core;

    class Object : public behavior::basic::UniqueId<Id>
    {
        friend class ObjectManager;
        friend class WorldManager;
    public:
        virtual ~Object() = default;
        bool dead() const;
    protected:
        Object() = default;
        ObjectManager* object_manager();
        void set_dead();
    private:
        ObjectManager* _object_manager {nullptr};
        bool _dead{false};
    };



}

#endif //ENGINE_BASICOBJECTS_H
