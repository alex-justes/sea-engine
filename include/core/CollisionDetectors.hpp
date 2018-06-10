#ifndef ENGINE_COLLISIONDETECTORS_HPP
#define ENGINE_COLLISIONDETECTORS_HPP

#include <set>
#include <map>
#include <cmath>
#include <algorithm>
#include "helpers/Containers.hpp"
#include "core/BasicBehaviors.hpp"
#include "Log.h"

namespace core::collision_detector
{
    template<class T, template<class> class Behavior = core::behavior::CollisionShape>
    class BroadAABBCollisionDetector
    {
    public:
        using value_type = T;
        using behavior_type = Behavior<AABB>;
        using Id = typename T::unique_id_type;
        using SingleCollisions = std::set<Id>;
        using CollisionPair = std::pair<uint32_t, uint32_t>;
        using PairCollisions = std::set<CollisionPair>;
        static constexpr bool check_traits();
        BroadAABBCollisionDetector();

        virtual void add(const T &object) = 0;
        virtual void remove(const T &object) = 0;
        virtual void remove(Id id) = 0;
        virtual void update(const T &object) = 0;
        virtual void update(Id id) = 0;
        virtual SingleCollisions broad_check(const Point &pt) = 0;
        virtual SingleCollisions broad_check(const Rect &rc) = 0;
        virtual PairCollisions broad_check() = 0;

        virtual ~BroadAABBCollisionDetector() = default;

    protected:
        static const AABB &get_shape(const T &object)
        {
            if constexpr (std::is_same_v<behavior_type, core::behavior::CollisionShape<AABB>>)
            {
                return object.collision_shape();
            } else if constexpr (std::is_same_v<behavior_type, core::behavior::RenderShape<AABB>>)
            {
                return object.render_shape();
            }
            // NOTE: this code should never be called in normal circumstances
            assert(false);
        }
    private:
    };

    template<class T, template<class> class Behavior>
    BroadAABBCollisionDetector<T, Behavior>::BroadAABBCollisionDetector()
    {
        static_assert(check_traits(), "Traits check failed");
    }

    template<class T, template<class> class Behavior>
    constexpr bool BroadAABBCollisionDetector<T, Behavior>::check_traits()
    {
        constexpr bool has_collision_shape = std::is_base_of_v<core::behavior::CollisionShape<AABB>, behavior_type> ||
                                             std::is_base_of_v<core::behavior::RenderShape<AABB>, behavior_type>;
        constexpr bool has_unique_id = std::is_base_of_v<core::behavior::IUniqueId, T>;
        return has_collision_shape && has_unique_id;
    }


    template<class T, template<class> class Behavior = core::behavior::CollisionShape>
    class HierarchicalSpatialGrid : public BroadAABBCollisionDetector<T, Behavior>
    {
    public:
        using Id = typename T::unique_id_type;
        using value_type = typename BroadAABBCollisionDetector<T, Behavior>::value_type;
        using behavior_type = typename BroadAABBCollisionDetector<T, Behavior>::behavior_type;
        using Size = helpers::containers::Size2D<uint32_t>;
        using SingleCollisions = typename BroadAABBCollisionDetector<T, Behavior>::SingleCollisions;
        using CollisionPair = typename BroadAABBCollisionDetector<T, Behavior>::CollisionPair;
        using PairCollisions = typename BroadAABBCollisionDetector<T, Behavior>::PairCollisions;

        HierarchicalSpatialGrid() = default;

        void add(const T &object) override;
        void remove(const T &object) override;
        void remove(Id id) override;
        SingleCollisions broad_check(const Point &pt) override;
        void update(const T &object) override;
        void update(Id id) override;
        SingleCollisions broad_check(const Rect &rc) override;
        PairCollisions broad_check() override;

        void set_world_size(const Size &size);

    private:
        using Map = std::set<Id>;
        using Grid = helpers::containers::Matrix<Map, 2>;
        using GridMap = std::map<uint32_t, Grid>;

        static uint32_t calc_level(const AABB &shape);
        static uint32_t calc_cell_size(uint32_t level);
        Rect calc_roi(uint32_t level, const AABB &shape) const;
        static Point calc_cell(uint32_t level, const Point &pt);

        struct ObjectInfo
        {
            const T *object{nullptr};
            uint32_t level{0};
            Rect roi{0, 0, 0, 0};
        };

        Size _world_size{0, 0};
        GridMap _grid_map;
        std::map<Id, ObjectInfo> _objects;
        std::map<uint32_t, uint32_t> _objects_per_level;
    };


    template<class T, template<class> class Behavior>
    void HierarchicalSpatialGrid<T, Behavior>::set_world_size(const Size &size)
    {
        _world_size = size;
    }

    template<class T, template<class> class Behavior>
    uint32_t HierarchicalSpatialGrid<T, Behavior>::calc_level(const AABB &shape)
    {
        auto max_side = std::max(shape.width(), shape.height());
        return (uint32_t) (std::log2(max_side));
    }

    template<class T, template<class> class Behavior>
    uint32_t HierarchicalSpatialGrid<T, Behavior>::calc_cell_size(uint32_t level)
    {
        return (uint32_t) (std::exp2(level + 1));
    }

    template<class T, template<class> class Behavior>
    Rect HierarchicalSpatialGrid<T, Behavior>::calc_roi(uint32_t level, const AABB &shape) const
    {
        auto cell_size = calc_cell_size(level);
        Point left_top(shape.top_left.x / cell_size, shape.top_left.y / cell_size);
        Point right_bottom(shape.bottom_right.x / cell_size, shape.bottom_right.y / cell_size);
        return Rect(left_top, right_bottom);
    }

    template<class T, template<class> class Behavior>
    Point HierarchicalSpatialGrid<T, Behavior>::calc_cell(uint32_t level, const Point &pt)
    {
        auto cell_size = calc_cell_size(level);
        return Point(pt.x / cell_size, pt.y / cell_size);
    }

    template<class T, template<class> class Behavior>
    void HierarchicalSpatialGrid<T, Behavior>::add(const T &object)
    {
        auto id = object.unique_id();
        if (_objects.count(id) > 0)
        {
            return;
        }

        auto level = calc_level(this->get_shape(object));
        auto roi = calc_roi(level, this->get_shape(object));

        _objects[id] = ObjectInfo();
        _objects[id].object = &object;
        _objects[id].level = level;
        _objects[id].roi = roi;

        if (_grid_map.count(level) == 0)
        {
            auto cell_size = calc_cell_size(level);
            _grid_map[level] = Grid();
            _grid_map[level].create({_world_size.y / cell_size + 1, _world_size.x / cell_size + 1});
            _objects_per_level[level] = 0;
        }
        LOG_D("Add: %d to l: %d", id, level)
        for (uint32_t y = roi.top_left.y; y <= roi.bottom_right.y; ++y)
        {
            for (uint32_t x = roi.top_left.x; x <= roi.bottom_right.x; ++x)
            {
                LOG_D("      x: %d y: %d", x, y)
                _grid_map[level][y][x].insert(id);
            }
        }
        _objects_per_level[level]++;
    }

    template<class T, template<class> class Behavior>
    void HierarchicalSpatialGrid<T, Behavior>::remove(const T &object)
    {
        auto id = object.unique_id();
        remove(id);
    }

    template<class T, template<class> class Behavior>
    void HierarchicalSpatialGrid<T, Behavior>::remove(Id id)
    {
        if (_objects.count(id) == 0)
        {
            return;
        }
        LOG_D("Remove: %d", id)
        auto object_info = _objects[id];
        auto level = object_info.level;
        auto roi = object_info.roi;
        for (uint32_t y = roi.top_left.y; y <= roi.bottom_right.y; ++y)
        {
            for (uint32_t x = roi.top_left.x; x <= roi.bottom_right.x; ++x)
            {
                _grid_map[level][y][x].erase(id);
            }
        }
        _objects_per_level[level]--;
        if (_objects_per_level[level] == 0)
        {
            _grid_map.erase(level);
        }
        _objects.erase(id);
    }

    template<class T, template<class> class Behavior>
    void HierarchicalSpatialGrid<T, Behavior>::update(const T &object)
    {
        auto id = object.unique_id();
        update(id);
    }

    template<class T, template<class> class Behavior>
    void HierarchicalSpatialGrid<T, Behavior>::update(Id id)
    {
        if (_objects.count(id) == 0)
        {
            return;
        }
        const auto &object = *(_objects[id].object);
        auto level = calc_level(this->get_shape(object));
        auto roi = calc_roi(level, this->get_shape(object));
        if (level != _objects[id].level || roi != _objects[id].roi)
        {
            remove(id);
            add(object);
        }
    }
    template<class T, template<class> class Behavior>
    typename HierarchicalSpatialGrid<T, Behavior>::SingleCollisions
    HierarchicalSpatialGrid<T, Behavior>::broad_check(const Point &pt)
    {
        SingleCollisions collisions;
        Point fixed_pt = {
                std::clamp(pt.x, (uint32_t) 0, _world_size.x),
                std::clamp(pt.y, (uint32_t) 0, _world_size.y)
        };
        for (const auto &l: _grid_map)
        {
            const auto&[level, grid] = l;
            auto cell = calc_cell(level, pt);
            for (const auto &o: grid[cell.y][cell.x])
            {
                auto shape = this->get_shape(*_objects[o].object);
                if (pt.x < shape.bottom_right.x && pt.x > shape.top_left.x
                    &&
                    pt.y < shape.bottom_right.y && pt.y > shape.top_left.y)
                {
                    collisions.insert(o);
                }
            }
        }
        return collisions;
    }

    template<class T, template<class> class Behavior>
    typename HierarchicalSpatialGrid<T, Behavior>::SingleCollisions
    HierarchicalSpatialGrid<T, Behavior>::broad_check(const Rect &rc)
    {
        SingleCollisions collisions;
        Rect fixed_rc = {
                std::clamp(rc.top_left.y, (uint32_t) 0, _world_size.y),
                std::clamp(rc.top_left.x, (uint32_t) 0, _world_size.x),
                std::clamp(rc.bottom_right.y, (uint32_t) 0, _world_size.y),
                std::clamp(rc.bottom_right.x, (uint32_t) 0, _world_size.x)
        };
        for (const auto &l: _grid_map)
        {
            const auto&[level, grid] = l;
            auto roi = calc_roi(level, fixed_rc);
            for (uint32_t y = roi.top_left.y; y <= roi.bottom_right.y; ++y)
            {
                for (uint32_t x = roi.top_left.x; x <= roi.bottom_right.x; ++x)
                {
                    for (const auto &o: grid[y][x])
                    {
                        if (fixed_rc && this->get_shape(*_objects[o].object))
                        {
                            collisions.insert(o);
                        }
                    }
                }
            }
        }
        return collisions;
    }

    template<class T, template<class> class Behavior>
    typename HierarchicalSpatialGrid<T, Behavior>::PairCollisions HierarchicalSpatialGrid<T, Behavior>::broad_check()
    {
        PairCollisions collisions;
        for (const auto &item: _objects)
        {
            const auto&[id, object_info] = item;
            auto single_collisions = broad_check(this->get_shape(*object_info.object));
            single_collisions.erase(id);
            for (const auto &collision: single_collisions)
            {
                auto first = std::min(id, collision);
                auto second = std::max(id, collision);
                collisions.insert(CollisionPair(first, second));
            }
        }
        return collisions;
    }
}
#endif //ENGINE_COLLISIONDETECTORS_HPP
