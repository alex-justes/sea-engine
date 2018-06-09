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

    class Rect: public SingleDrawable
    {
    public:
        using Roi = helpers::containers::Rect2D<uint32_t>;
        const Roi& roi() const;
        const RGBA& color() const;
    private:
        Roi _roi;
        RGBA _color;
    };
};

#endif //ENGINE_DRAWABLE_H
