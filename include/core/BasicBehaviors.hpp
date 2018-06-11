#ifndef ENGINE_BASICBEHAVIORS_HPP
#define ENGINE_BASICBEHAVIORS_HPP

#include "core/Types.h"
#include "UniqueIdGenerator.hpp"
#include "core/Drawable.h"
#include "helpers/Containers.hpp"

namespace core::behavior::basic
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
        void set_collision_shape(const Shape& shape)
        { _collision_shape = shape; }
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
        uint32_t z_order() const;
        void set_z_order(uint32_t z_order);
    private:
        uint32_t _z_order {0};
    };

    class Changed: public virtual IBehavior
    {
    public:
        bool changed() const;
        void set_changed();
    private:
        bool _changed {false};
    };


    class Updatable: public virtual IBehavior
    {
    public:
        virtual bool update(bool force) = 0;
    };

}

#endif //ENGINE_BASICBEHAVIORS_HPP
