#ifndef ENGINE_BASICBEHAVIORS_HPP
#define ENGINE_BASICBEHAVIORS_HPP

#include "core/Types.h"
#include "UniqueIdGenerator.hpp"
#include "core/Drawable.h"
#include "helpers/Containers.hpp"

namespace core::behavior
{
    class IBehavior
    {
    public:
        virtual ~IBehavior()=default;
    };

    class IUniqueId : public virtual IBehavior
    {
    };

    template<class T>
    class UniqueId : public IUniqueId
    {
    public:
        using unique_id_type = T;
        UniqueId()
                :
                _unique_id(utils::UniqueIdGenerator<unique_id_type>::generate())
        {}
        virtual ~UniqueId()
        {
            utils::UniqueIdGenerator<unique_id_type>::free(_unique_id);
        }
        const unique_id_type &unique_id() const
        { return _unique_id; }
    private:
        const unique_id_type _unique_id;
    };

    template<class Shape>
    class CollisionShape : public virtual IBehavior
    {
    public:
        using collision_shape_type = Shape;
        const collision_shape_type &collision_shape() const
        { return _collision_shape; }
    protected:
        collision_shape_type &collision_shape()
        { return _collision_shape; }
    private:
        collision_shape_type _collision_shape;
    };

    template<class Shape>
    class RenderShape: public virtual IBehavior
    {
    public:
        using render_shape_type = Shape;
        const render_shape_type &render_shape() const
        { return _render_shape; }
    protected:
        render_shape_type &render_shape()
        { return _render_shape; }
    private:
        render_shape_type _render_shape;
    };

    class Position: public virtual IBehavior
    {
    public:
        const Point& position() const;
        Point& position();
    private:
        Point _position;
    };

    class Renderable:
            public RenderShape<helpers::containers::AABB>,
            public virtual Position
    {
    public:
        using Drawable = core::drawable::Drawable;
        uint32_t z_order() const;
        const Drawable *drawable() const;
        template <class T, class ... Types>
        T* set_drawable(Types &&... args);
    protected:
        Drawable *drawable();
    private:
        uint32_t _z_order {0};
        std::unique_ptr<Drawable> _drawable;
    };

    class Updatable: public virtual IBehavior
    {
    public:
        virtual bool update() = 0;
    };

    // ===============================================
    template<class T, class... Types>
    T *Renderable::set_drawable(Types &&... args)
    {
        static_assert(std::is_base_of_v<Drawable , T>, "T should be derived from Drawable");
        auto ptr = new T(std::forward<Types>(args)...);
        _drawable.reset(ptr);
        return ptr;
    }

}

#endif //ENGINE_BASICBEHAVIORS_HPP
