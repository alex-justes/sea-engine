#include <filesystem>
#include "Log.h"
#include "helpers/Configuration.h"
#include "core/Engine.h"
#include "core/Context.h"

namespace fs = std::filesystem;
using namespace core;

Engine::Engine(const fs::path &config)
{
    LOG_S("Reading configuration...")
    helpers::configuration::Configuration conf_reader;
    conf_reader.read(config);
    _config.window.width = conf_reader.get<int>("width", "window", 640);
    _config.window.height = conf_reader.get<int>("height", "window", 640);
    _config.window.caption = conf_reader.get<std::string>("caption", "window", "");
    _config.application.full_screen = conf_reader.get<bool>("full_screen", "application", false);
    _config.application.entry_point = conf_reader.get<std::string>("entry_point", "application", "");
    LOG_S("Done.")
}

Engine::~Engine()
{
    if (_window != nullptr)
    {
        SDL_DestroyWindow(_window);
    }
    SDL_Quit();
    LOG_S("Finished.")
}

bool Engine::initialize_sdl()
{
    LOG_S("Initializing SDL...")
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        LOG_E("Unable to initialize SDL: %s", SDL_GetError())
        LOG_S("Finished.")
        return false;
    }
    LOG_S("Done.")
    return true;
}

bool Engine::create_window()
{
    LOG_S("Creating main window...")
    _window = SDL_CreateWindow(_config.window.caption.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                               _config.window.width, _config.window.height,
                               _config.application.full_screen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0);
    if (_window == nullptr)
    {
        LOG_E("Unable to create Main Window: %s", SDL_GetError())
        return false;
    }
    return true;
}

bool Engine::create_renderer()
{
    LOG_S("Creating renderer....")
    _renderer = SDL_CreateRenderer(_window, -1, SDL_RENDERER_ACCELERATED);
    if (_renderer == nullptr)
    {
        LOG_E("Unable to create Renderer: %s", SDL_GetError())
        return false;
    }
    LOG_S("Done.")
    return true;
}

void Engine::main_loop()
{
    _running = true;
    EventManager event_manager;
    ScreenManager screen_manager;
    ContextManager context_manager;
    Context *context = context_manager.load_context(_config.application.entry_point.c_str(), event_manager,
                                                    screen_manager);
    if (context == nullptr)
    {
        LOG_E("Invalid context.")
        return;
    }

    LOG_S("Initializing context...")
    context->initialize();
    LOG_S("Done.")

    if (!create_window())
    {
        return;
    }

    if (!create_renderer())
    {
        return;
    }

    SDL_Event sdl_event;
    while (_running)
    {
        while (SDL_PollEvent(&sdl_event) != 0)
        {
            std::shared_ptr<Event> event;
            switch (sdl_event.type)
            {
                case SDL_QUIT:
                    _running = false;
                    LOG_S("Quit sdl_event.")
                case SDL_KEYDOWN:
                    event.reset(new KeyboardEvent(KeyboardEvent::State::PRESSED, sdl_event.key.keysym.sym,
                                                  sdl_event.key.keysym.scancode));
                    break;
                case SDL_KEYUP:
                    event.reset(new KeyboardEvent(KeyboardEvent::State::RELEASED, sdl_event.key.keysym.sym,
                                                  sdl_event.key.keysym.scancode));
                    break;
                default:
                    break;
            }
            if (!_running)
            {
                break;
            }
            if (event)
            {
                event_manager.push(event);
            }
        }
        context->evaluate();
    }
    context_manager.unload_context(context->unique_id());
}