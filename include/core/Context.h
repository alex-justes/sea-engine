#ifndef ENGINE_CONTEXT_H
#define ENGINE_CONTEXT_H

#include <queue>
#include <memory>
#include <map>
#include <set>
#include "core/Types.h"
#include "core/Events.h"
#include "core/BasicBehaviors.hpp"
#include "core/Screen.h"

namespace core
{
    class Engine;

    class Context;

    class ScreenManager
    {
        friend class Engine;

    public:
        ScreenManager(const ScreenManager &) = delete;
        ScreenManager &operator=(const ScreenManager &) = delete;
        Id create_screen(const Roi& roi, uint32_t z_order);
        void remove_screen(Id id);
        bool attach_camera(Camera* camera, Id screen);
        bool detach_camera(Camera* camera, Id screen);
    private:
        using Map = std::map<Id, std::unique_ptr<Screen>>;
        using const_iterator =  typename Map::const_iterator;
        using iterator = typename Map::iterator;

        explicit ScreenManager(SDL_Renderer* renderer);
        const_iterator cbegin() const;
        const_iterator cend() const;
        iterator begin();
        iterator end();
        SDL_Renderer* _renderer;
        Map _screens;
    };

    class EventManager
    {
    public:
        using Item = std::shared_ptr<const Event>;
        using EventQueue = std::queue<Item>;
        EventManager() = default;
        EventManager(const EventManager &) = delete;
        EventManager &operator=(const EventManager &) = delete;
        void subscribe(Context* context, EventType t);
        void unsubscribe(Context* context, EventType t);
        void unsubscribe(Context* context);
        void push(Item event);
    private:
        using Subscription = std::map<Id, Context*>;
        std::map<EventType, Subscription> _map;
    };

    typedef void *(*ContextFunction)(EventManager &, ScreenManager &);

    class ContextLoader
    {
    public:
        ContextLoader() = default;
        Context *load_context(const char *obj_file, EventManager &event_manager, ScreenManager &screen_manager);
        void unload_context(Id);
        virtual ~ContextLoader();
    private:
        struct ContextInfo
        {
            void *handle;
            Context *context;
        };
        std::map<Id, ContextInfo> _contexts;
    };

    class Context : public behavior::UniqueId<Id>
    {
        friend class EventManager;

    public:
        Context() = delete;
        Context(EventManager &event_manager, ScreenManager &screen_manager);
        virtual void evaluate(uint32_t time_elapsed) = 0;
        virtual void initialize() = 0;
        virtual void pause();
        virtual void unpause();
        bool paused() const;
        bool finished() const;
        virtual ~Context();
    protected:
        using Item = typename EventManager::Item;
        virtual void subscribe(EventType t) final;
        virtual void unsubscribe(EventType t) final;
        virtual bool events_pop(Item& event) final;
        void set_finished(bool status);
        ScreenManager& screen_manager();
    private:
        using EventQueue = typename EventManager::EventQueue;
        void enqueue(Item event);
        void subscribe_impl(EventType t);
        void unsubscribe_impl(EventType t);
        void unsubscribe_impl();
        EventManager &_external_event_manager;
        ScreenManager &_screen_manager;
        EventQueue _event_queue;
        bool _finished {true};
        bool _paused {false};
    };
}

#endif //ENGINE_CONTEXT_H
