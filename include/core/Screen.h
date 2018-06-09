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

    class Screen: public behavior::UniqueId<uint32_t>
    {
        friend class ScreenManager;
    public:
        Screen() = delete;
        SDL_Texture* render();
        void attach_camera(const Camera* camera);
        void detach_camera();
        const Roi& roi() const;
        int z_order() const;
        virtual ~Screen();
    protected:
        using PointI32 = helpers::containers::Point2D<int32_t>;
        Screen(const Roi& roi, uint32_t z_order, SDL_Renderer* renderer);
        // TODO: fix type
        void render(const drawable::Drawable* drawable, const PointI32& position, float scale);
        void change_z_order(uint32_t z_order);
    private:
        Roi _roi;
        SDL_Texture* _texture {nullptr};
        SDL_Renderer* _renderer {nullptr};
        const Camera* _camera {nullptr};
        uint32_t _z_order {0};
    };
}

#endif //ENGINE_SCREEN_H
