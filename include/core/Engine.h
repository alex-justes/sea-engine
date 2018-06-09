#ifndef ENGINE_ENGINE_H
#define ENGINE_ENGINE_H

#include <string>
#include <filesystem>
#include "core/Context.h"
// TODO: strange behavior (simple #include "SDL.h" do not work)
#include "SDL2/SDL.h"

namespace core
{

    class Engine
    {
    public:
        explicit Engine(const std::filesystem::path &config);
        bool initialize_sdl();
        void main_loop();
        virtual ~Engine();
    private:
        bool create_window();
        bool create_renderer();
        bool _running{false};
        struct Configuration
        {
            struct
            {
                int width{0};
                int height{0};
                std::string caption;
            } window;
            struct
            {
                bool full_screen{false};
                int fps{0};
                std::string entry_point;
            } application;
        } _config;
        SDL_Window* _window {nullptr};
        SDL_Renderer* _renderer {nullptr};
    };
}

#endif //ENGINE_ENGINE_H
