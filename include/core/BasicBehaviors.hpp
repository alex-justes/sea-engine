#ifndef ENGINE_BASICBEHAVIORS_HPP
#define ENGINE_BASICBEHAVIORS_HPP

#include "core/Types.h"
#include "UniqueIdGenerator.hpp"
#include "helpers/Containers.hpp"

namespace core::basic::behavior
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

    class StaticShape : public virtual IBehavior
    {
    public:
        bool is_static_shape() const;
        void set_static_shape(bool static_shape);
    private:
        bool _static{false};
    };

    template<class Shape>
    class CollisionShape :
            public virtual IBehavior,
            public virtual StaticShape
    {
    public:
        using collision_shape_type = Shape;
        const collision_shape_type &collision_shape() const
        { return _collision_shape; }
        void set_collision_shape(const Shape& shape)
        { _collision_shape = shape; }
    private:
        collision_shape_type _collision_shape;
    };

    template <class T>
    class CollisionSize : public virtual IBehavior
    {
    public:
        const T& collision_size() const
        { return _size; }
        void set_collision_size(const T& size)
        { _size = size; }
    private:
        T _size;
    };

    template<class Shape>
    class RenderShape:
            public virtual IBehavior,
            public virtual StaticShape
    {
    public:
        using render_shape_type = Shape;
        const render_shape_type &render_shape() const
        { return _render_shape; }
        void set_render_shape(const render_shape_type& shape)
        { _render_shape = shape; }
    private:
        render_shape_type _render_shape;
    };

    class Position: public virtual IBehavior
    {
    public:
        const Point& position() const;
        virtual void set_position(const Point& position);
    private:
        Point _position;
    };

    class Z_Order: public virtual IBehavior
    {
    public:
        int32_t z_order() const;
        void set_z_order(int32_t z_order);
    private:
        int32_t _z_order {0};
    };

    class Changed: public virtual IBehavior
    {
    public:
        bool changed() const;
        void set_changed(bool changed);
    private:
        bool _changed {false};
    };

    class Dead: public virtual IBehavior
    {
    public:
        bool dead() const;
        void set_dead();
    private:
        bool _dead {false};
    };

    class BoxSize: public virtual IBehavior
    {
    public:
        const Size& box_size() const;
        void set_box_size(const Size& size);
    private:
        Size _size;
    };

    class FillColor: public virtual IBehavior
    {
    public:
        const RGBA& fill_color() const;
        void set_fill_color(const RGBA& color);
    protected:
        RGBA _color;
    };

    class BorderColor: public virtual IBehavior
    {
    public:
        const RGBA& border_color() const;
        void set_border_color(const RGBA& color);
    protected:
        RGBA _color;
    };

}

#endif //ENGINE_BASICBEHAVIORS_HPP
