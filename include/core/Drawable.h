#ifndef ENGINE_DRAWABLE_H
#define ENGINE_DRAWABLE_H

#include <memory>
#include <list>
#include "helpers/Containers.hpp"

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
        const Drawable* get_drawable() const;
    };

    class DrawableShape
    {
    public:
        virtual ~DrawableShape() = default;
    };

    class RectShape: public DrawableShape
    {
    public:
        using Rect = helpers::containers::Rect2D<uint32_t>;
        const Rect& rect() const;
        const RGBA& color() const;
    private:
        Rect _rect;
        RGBA _color;
    };

    class SingleDrawable: public Drawable
    {
    public:
        const DrawableShape* shape() const;
    private:
        std::unique_ptr<DrawableShape> _shape;
    };

    class CompoundDrawable: public Drawable
    {
    public:
        std::list<const Drawable*> get_drawables() const;
    private:

    };
};

#endif //ENGINE_DRAWABLE_H
