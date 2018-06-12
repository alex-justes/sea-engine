#include <filesystem>
#include <chrono>
#include <core/Engine.h>
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
    _config.application.fps = conf_reader.get<int>("fps", "application", 60);
    if (_config.application.fps < 1)
    {
        _config.application.fps = 60;
    }
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

std::shared_ptr<Event> Engine::parse_mouse_event(const SDL_Event &sdl_event, const ScreenManager &screen_manager)
{
    Point pt;
    std::shared_ptr<Event> event;
    MouseClickEvent::State state = MouseClickEvent::State::PRESSED;
    switch (sdl_event.type)
    {
        case SDL_MOUSEBUTTONDOWN:
            pt = {sdl_event.button.x, sdl_event.button.y};
            state = MouseClickEvent::State::PRESSED;
            break;
        case SDL_MOUSEBUTTONUP:
            pt = {sdl_event.button.x, sdl_event.button.y};
            state = MouseClickEvent::State::RELEASED;
            break;
        case SDL_MOUSEMOTION:
            pt = {sdl_event.motion.x, sdl_event.motion.y};
            break;
        default:
            break;
    }

    auto screen = screen_manager.find_screen(pt);
    if (screen != nullptr && screen->accept_mouse_input())
    {
        pt -= screen->roi().top_left;
        auto camera_pt = screen->to_camera_coords(pt);
        auto context_id = static_cast<const Context*>(screen->camera()->current_context())->unique_id();
        auto camera_id = screen->camera()->unique_id();
        switch (sdl_event.type)
        {
            case SDL_MOUSEMOTION:
                event.reset(new MouseMoveEvent(camera_pt, context_id, camera_id));
                break;
            default:
            {
                bool status = true;
                MouseClickEvent::Button button = MouseClickEvent::Button::LEFT;
                switch (sdl_event.button.button)
                {
                    case SDL_BUTTON_LEFT:
                        button = MouseClickEvent::Button::LEFT;
                        break;
                    case SDL_BUTTON_RIGHT:
                        button = MouseClickEvent::Button::RIGHT;
                        break;
                    default:
                        status = false;
                        break;
                }
                if (status)
                {
                    event.reset(new MouseClickEvent(camera_pt, button, state, context_id, camera_id));
                }
            }
            break;
        }
    }
    return event;
}

std::shared_ptr<Event> Engine::parse_event(const SDL_Event &sdl_event, const ScreenManager &screen_manager)
{
    std::shared_ptr<Event> event;
    switch (sdl_event.type)
    {
        case SDL_KEYDOWN:
            event.reset(new KeyPressEvent(KeyPressEvent::State::PRESSED, sdl_event.key.keysym.sym,
                                          sdl_event.key.keysym.scancode));
            break;
        case SDL_KEYUP:
            event.reset(new KeyPressEvent(KeyPressEvent::State::RELEASED, sdl_event.key.keysym.sym,
                                          sdl_event.key.keysym.scancode));
            break;
        case SDL_MOUSEMOTION:
        case SDL_MOUSEBUTTONDOWN:
        case SDL_MOUSEBUTTONUP:
            event = std::move(parse_mouse_event(sdl_event, screen_manager));
            break;
        default:
            break;
    }
    return event;
}
void Engine::main_loop()
{
    _running = true;
    EventManager event_manager;
    ContextLoader context_manager;

    if (!create_window())
    {
        return;
    }

    if (!create_renderer())
    {
        return;
    }

    ScreenManager screen_manager(_renderer);

    SDL_SetRenderDrawBlendMode(_renderer, SDL_BLENDMODE_BLEND);

    int window_w, window_h;
    SDL_GetWindowSize(_window, &window_w, &window_h);
    screen_manager.set_screen_size(Size{window_w, window_h});

    Context *context = context_manager.load_context(_config.application.entry_point.c_str(),
                                                    event_manager,
                                                    screen_manager);
    if (context == nullptr)
    {
        LOG_E("Invalid context.")
        return;
    }

    LOG_S("Initializing context...")
    context->initialize();
    LOG_S("Done.")

    using namespace std::chrono;
    auto desired_frame_duration = milliseconds(1000 / _config.application.fps);
    SDL_Event sdl_event;
    while (_running && !context->finished())
    {
        auto frame_start_time = steady_clock::now();
        while (SDL_PollEvent(&sdl_event) != 0)
        {
            if (sdl_event.type == SDL_QUIT)
            {
                _running = false;
                LOG_S("Quit event")
            }
            std::shared_ptr<Event> event = std::move(parse_event(sdl_event, screen_manager));
            if (!_running)
            {
                break;
            }
            if (event)
            {
                event_manager.dispatch(event);
            }
        }
        context->evaluate((uint32_t) (desired_frame_duration.count()));
        std::multimap<uint32_t, Screen *> screens;
        for (auto &item: screen_manager)
        {
            Screen *screen = item.second.get();
            screens.emplace(screen->z_order(), screen);
        }
        SDL_SetRenderDrawColor(_renderer, 0, 0, 0, 0);
        SDL_RenderClear(_renderer);
        for (auto &item: screens)
        {
            Screen *screen = item.second;
            SDL_Rect from = {0, 0, (int32_t) screen->roi().width(), (int32_t) screen->roi().height()};
            SDL_Rect to = {(int32_t) screen->roi().top_left.x, (int32_t) screen->roi().top_left.y,
                           (int32_t) screen->roi().width(), (int32_t) screen->roi().height()};
            SDL_RenderCopy(_renderer, screen->render(), &from, &to);
        }
        SDL_RenderPresent(_renderer);
        auto frame_end_time = steady_clock::now();
        auto frame_duration = frame_end_time - frame_start_time;
        auto delta = duration_cast<milliseconds>(desired_frame_duration - frame_duration);
        if (delta.count() > 0)
        {
            SDL_Delay((uint32_t) (delta.count()));
        }
    }
    context_manager.unload_context(context->unique_id());
}