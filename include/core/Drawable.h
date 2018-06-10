#ifndef ENGINE_DRAWABLE_H
#define ENGINE_DRAWABLE_H

#include <memory>
#include <list>
#include "helpers/Containers.hpp"
#include "core/Types.h"

namespace core::drawable
{
    struct RGBA
    {
        uint8_t r;
        uint8_t g;
        uint8_t b;
        uint8_t a;
    };

    class Drawable
    {
    public:
        Drawable() = default;
        virtual ~Drawable() = default;
        virtual AABB bounding_box() const = 0;
    };

    class SingleDrawable: public Drawable
    {
    };

    class CompoundDrawable: public Drawable
    {
    public:
        std::list<const Drawable*> get_drawables() const;
    private:

    };

    class DrawableRect: public SingleDrawable
    {
    public:
        const Size& size() const;
        const RGBA& fill_color() const;
        const RGBA& border_color() const;
        Size& size();
        RGBA& fill_color();
        RGBA& border_color();
        virtual AABB bounding_box() const override;
    private:
        Size _size;
        RGBA _fill_color;
        RGBA _border_color;
    };

};

#endif //ENGINE_DRAWABLE_H
