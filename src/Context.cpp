#include <list>
#include "SDL.h"
#include "core/Context.h"
#include "Log.h"

using namespace core;

ScreenManager::ScreenManager(SDL_Renderer *renderer)
        :
        _renderer(renderer)
{

}

Id ScreenManager::create_screen(const Roi &roi, int32_t z_order, bool accept_mouse_input, const RGBA& base_color)
{
    auto screen = new Screen(roi, z_order, _renderer, base_color);
    screen->set_accept_mouse_input(accept_mouse_input);
    _screens.emplace(screen->unique_id(), screen);
    _detector.add(*screen);
    return screen->unique_id();
}

void ScreenManager::remove_screen(Id id)
{
    _detector.remove(id);
    _screens.erase(id);
}

bool ScreenManager::attach_camera(core::Camera *camera, Id screen)
{
    if (camera == nullptr || _screens.count(screen) == 0)
    {
        return false;
    }
    if (_screens[screen]->camera_attached())
    {
        _screens[screen]->_camera->_screens_attached--;
        _screens[screen]->_camera = nullptr;
    }
    _screens[screen]->attach_camera(camera);
    camera->_screens_attached++;
    return true;
}

bool ScreenManager::detach_camera(core::Camera *camera, Id screen)
{
    if (camera == nullptr || _screens.count(screen) == 0)
    {
        return false;
    }
    _screens[screen]->detach_camera();
    camera->_screens_attached--;
    return true;
}

bool ScreenManager::detach_camera(core::Camera *camera)
{
    if (camera == nullptr)
    {
        return false;
    }
    bool detached = false;
    for (auto& item: _screens)
    {
        auto& [id, screen] = item;
        if (screen->camera_attached() && camera->unique_id() == screen->_camera->unique_id())
        {
            detach_camera(camera, id);
            detached = true;
            break;
        }
    }
    return detached;
}

const Size& ScreenManager::screen_size() const
{
    return _screen_size;
}

const Screen *ScreenManager::find_screen(const Point &point) const
{
    auto screens = _detector.broad_check(point);
    if (screens.empty())
    {
        return nullptr;
    }
    auto id = *(--screens.end());
    auto screen = _screens.find(id);
    return screen->second.get();
}

void ScreenManager::set_screen_size(const Size &size)
{
    _screen_size = size;
    _detector.set_world_size(size);
}

ScreenManager::const_iterator ScreenManager::cbegin() const
{
    return _screens.cbegin();
}
ScreenManager::const_iterator ScreenManager::cend() const
{
    return _screens.cend();
}

ScreenManager::iterator ScreenManager::begin()
{
    return _screens.begin();
}
ScreenManager::iterator ScreenManager::end()
{
    return _screens.end();
}

Context *ContextLoader::load_context(const char *obj_file, EventManager &event_manager, ScreenManager &screen_manager)
{
    LOG_S("Loading context from %s", obj_file)
    void *handle = SDL_LoadObject(obj_file);
    if (handle == nullptr)
    {
        LOG_E("Unable to load context %s", SDL_GetError())
        return nullptr;
    }
    auto func = (ContextFunction) SDL_LoadFunction(handle, "create_context");
    if (func == nullptr)
    {
        LOG_E("Unable to load context %s", SDL_GetError())
        return nullptr;
    }
    LOG_S("Done.")
    auto context = static_cast<Context *>(func(event_manager, screen_manager));
    auto id = context->unique_id();
    ContextInfo info{handle, context};
    _contexts.insert(std::make_pair(id, info));
    return context;
}

void ContextLoader::unload_context(Id id)
{
    auto it = _contexts.find(id);
    if (it == _contexts.end())
    {
        return;
    }
    auto &info = it->second;
    LOG_D("Unloading context %d", it->first)
    delete info.context;
    SDL_UnloadObject(info.handle);
    _contexts.erase(it);
}

ContextLoader::~ContextLoader()
{
    for (auto &item: _contexts)
    {
        auto&[id, info] = item;
        LOG_D("Unloading context %d", id)
        delete info.context;
        SDL_UnloadObject(info.handle);
    }
    _contexts.clear();
}

void EventManager::subscribe(Context *context, EventType t)
{
    if (_map.count(t) == 0)
    {
        _map[t] = Subscription();
    }
    Id id = context->unique_id();
    _map[t][id] = context;
}

void EventManager::unsubscribe(Context *context, EventType t)
{
    if (_map.count(t) == 0)
    {
        return;
    }
    Id id = context->unique_id();
    _map[t].erase(id);
    if (_map[t].empty())
    {
        _map.erase(t);
    }
}

void EventManager::unsubscribe(Context *context)
{
    std::list<EventType> available_events;
    for (const auto &item: _map)
    {
        available_events.push_back(item.first);
    }
    for (const auto &event: available_events)
    {
        unsubscribe(context, event);
    }
}

void EventManager::dispatch(Item event)
{
    if (_map.count(event->type()) == 0)
    {
        return;
    }
    for (auto &item: _map[event->type()])
    {
        auto&[id, context] = item;
        context->enqueue(event);
    }
}

Context::Context(EventManager &event_manager, ScreenManager &screen_manager)
        :
        _external_event_manager(event_manager),
        _screen_manager(screen_manager)
{

}

Context::~Context()
{
    unsubscribe_impl();
}

ScreenManager& Context::screen_manager()
{
    return _screen_manager;
}

void Context::subscribe(EventType t)
{
    subscribe_impl(t);
}

void Context::enqueue(Item event)
{
    _event_queue.emplace(std::move(event));
}

void Context::subscribe_impl(EventType t)
{
    _external_event_manager.subscribe(this, t);
}

void Context::unsubscribe(EventType t)
{
    unsubscribe_impl(t);
}

void Context::unsubscribe_impl(EventType t)
{
    _external_event_manager.unsubscribe(this, t);
}

void Context::unsubscribe_impl()
{
    _external_event_manager.unsubscribe(this);
}
bool Context::events_pop(Item &event)
{
    event.reset();
    if (!_event_queue.empty())
    {
        event = std::move(_event_queue.back());
        _event_queue.pop();
        return true;
    }
    return false;
}

bool Context::finished() const
{
    return _finished;
}

bool Context::paused() const
{
    return _paused;
}

void Context::pause()
{
    _paused = true;
}

void Context::unpause()
{
    _paused = false;
}

void Context::set_finished(bool status)
{
    _finished = status;
}
