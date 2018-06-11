#ifndef ENGINE_SCREEN_H
#define ENGINE_SCREEN_H

#include "helpers/Containers.hpp"
#include "core/BasicBehaviors.hpp"
#include "core/Camera.h"
#include "core/Types.h"
// TODO: strange behavior (simple #include "SDL.h" do not work)
#include "SDL2/SDL.h"

namespace core
{
    class ScreenManager;

    class Screen:
            public virtual basic::behavior::UniqueId<Id>,
            public virtual basic::behavior::Z_Order
    {
        friend class ScreenManager;
    public:
        Screen() = delete;
        SDL_Texture* render();
        const Roi& roi() const;
        virtual ~Screen();
    protected:
        void attach_camera(Camera* camera);
        void detach_camera();
        bool camera_attached();
        Screen(const Roi& roi, int32_t z_order, SDL_Renderer* renderer, const RGBA& base_color);
        void render(const drawable::Drawable* drawable, const PointI32& position);
    private:
        Roi _roi;
        SDL_Texture* _texture {nullptr};
        SDL_Renderer* _renderer {nullptr};
        Camera* _camera {nullptr};
        uint32_t _z_order {0};
        RGBA _base_color{0,0,0,0};
    };
}

#endif //ENGINE_SCREEN_H
