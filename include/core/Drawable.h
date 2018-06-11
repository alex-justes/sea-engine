#ifndef ENGINE_DRAWABLE_H
#define ENGINE_DRAWABLE_H

#include <memory>
#include <list>
#include "helpers/Containers.hpp"
#include "core/BasicBehaviors.hpp"
#include "core/Types.h"

namespace core::drawable
{
    class Drawable
    {
    public:
        Drawable() = default;
        virtual ~Drawable() = default;
        virtual AABB bounding_box() const = 0;
    };

    class SingleDrawable : public Drawable
    {
    };

    class CompoundDrawable : public Drawable
    {
    public:
        std::list<const Drawable *> get_drawables() const;
    private:

    };

    class DrawableRect :
            public SingleDrawable,
            public virtual core::basic::behavior::BoxSize,
            public virtual core::basic::behavior::FillColor,
            public virtual core::basic::behavior::BorderColor
    {
    public:
        AABB bounding_box() const override;
    };

};

#endif //ENGINE_DRAWABLE_H
