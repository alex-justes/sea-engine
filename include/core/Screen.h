#ifndef ENGINE_SCREEN_H
#define ENGINE_SCREEN_H

#include "helpers/Containers.hpp"
#include "core/BasicBehaviors.hpp"
#include "core/Camera.h"
#include "SDL.h"

namespace core
{
    class ScreenManager;

    class Screen: public behavior::UniqueId<uint32_t>
    {
        friend class ScreenManager;
    public:
        Screen() = delete;
        using Roi = helpers::containers::Rect2D<uint32_t>;
        using Point = helpers::containers::Point2D<uint32_t>;
        const SDL_Texture* render();
        int z_order() const;
        virtual ~Screen();
    protected:
        Screen(const Roi& roi, int z_order, SDL_Renderer* renderer);
        void render(const drawable::Drawable* drawable, const Point& position, float scale);
        void change_z_order(int z_order);
    private:
        Roi _roi;
        SDL_Texture* _texture {nullptr};
        SDL_Renderer* _renderer {nullptr};
        const Camera* _camera {nullptr};
        int _z_order {0};
    };
}

#endif //ENGINE_SCREEN_H
