#ifndef ENGINE_CAMERA_H
#define ENGINE_CAMERA_H

#include <list>
#include <map>
#include "core/BasicBehaviors.hpp"
#include "core/Types.h"

namespace core
{
    class ScreenManager;
    class CameraManager;

    class Camera:
            public behavior::Position,
            public behavior::UniqueId<Id>
    {
        friend class ScreenManager;
        friend class CameraManager;
    public:
        using ObjectType = const behavior::Renderable*;
        using List = std::list<ObjectType>;
        void update_visible_objects(List&& list);
        const List& get_visible_objects() const;
        const Size& size() const;
        bool active();
        virtual ~Camera();
    protected:
        Camera() = delete;
        Camera(const Point& position, const Size& size);
        Size& size();
    private:
        uint32_t _screens_attached {0};
        Size _size;
        List _objects;
    };

    class CameraManager
    {
        friend class WorldManager;
    private:
        using Item = std::unique_ptr<core::Camera>;
        using Cameras = std::map<Id, Item>;
    public:
        using const_iterator = typename Cameras::const_iterator;
        using iterator = typename Cameras::iterator;

        Camera* create_camera(const Point& position, const Size& size);
        void remove_camera(Id camera);
        Camera* get_camera(Id camera);

        const_iterator cbegin() const;
        const_iterator cend() const;
        iterator begin();
        iterator end();

        virtual ~CameraManager() = default;
    protected:
    private:
        Cameras _cameras;
    };
}

#endif //ENGINE_CAMERA_H
