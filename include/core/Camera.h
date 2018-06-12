#ifndef ENGINE_CAMERA_H
#define ENGINE_CAMERA_H

#include <list>
#include <map>
#include "core/ComplexBehaviors.hpp"
#include "core/Types.h"

namespace core
{
    class ScreenManager;
    class CameraManager;

    class Camera:
            public virtual basic::behavior::Position,
            public virtual basic::behavior::UniqueId<Id>
    {
        friend class ScreenManager;
        friend class CameraManager;
    public:
        using ObjectType = const complex::behavior::Renderable*;
        using List = std::list<ObjectType>;
        void update_visible_objects(List&& list);
        const List& get_visible_objects() const;
        const Size& size() const;
        bool active();
        const void* current_context() const;
        virtual ~Camera();
    protected:
        Camera() = delete;
        Camera(const Point& position, const Size& size);
        void set_size(const Size& size);
        void set_current_context(const void* context);
    private:
        uint32_t _screens_attached {0};
        Size _size;
        List _objects;
        const void* _current_context;
    };


    // TODO: friend world manager ?
    class CameraManager
    {
        friend class helpers::context::WorldManager;
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

        virtual ~CameraManager() = default;
    protected:
        iterator begin();
        iterator end();
        void set_current_context(const void* context);
    private:
        Cameras _cameras;
        const void* _current_context;
    };
}

#endif //ENGINE_CAMERA_H
