#ifndef ENGINE_SCREEN_H
#define ENGINE_SCREEN_H

#include "helpers/Containers.hpp"
#include "core/BasicBehaviors.hpp"
#include "core/Camera.h"
#include "core/Types.h"
#include "core/AffineTransformation.h"
// TODO: strange behavior (simple #include "SDL.h" do not work)
#include "SDL2/SDL.h"

namespace core
{
    class ScreenManager;

    class Screen:
            public virtual basic::behavior::UniqueId<Id>,
            public virtual basic::behavior::Z_Order,
            public virtual basic::behavior::CollisionShape<AABB>
    {
        friend class ScreenManager;
    public:
        Screen() = delete;
        const Camera* camera() const;
        SDL_Texture* render();
        const Roi& roi() const;
        virtual ~Screen();
        bool accept_mouse_input() const;
        Point to_camera_coords(const PointF& pt) const;
    protected:
        void attach_camera(Camera* camera);
        void detach_camera();
        bool camera_attached();
        Screen(const Roi& roi, int32_t z_order, SDL_Renderer* renderer, const RGBA& base_color);
        void render(const drawable::Drawable* drawable, const PointI32& position);
        void set_accept_mouse_input(bool flag);
    private:
        Roi _roi;
        SDL_Texture* _texture {nullptr};
        SDL_Renderer* _renderer {nullptr};
        Camera* _camera {nullptr};
        uint32_t _z_order {0};
        RGBA _base_color{0,0,0,0};
        bool _accept_mouse_input{true};
        transformation::AffineTransformation _screen_to_camera;
        transformation::AffineTransformation _camera_to_screen;
    };
}

#endif //ENGINE_SCREEN_H
