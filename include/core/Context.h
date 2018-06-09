#ifndef ENGINE_CONTEXT_H
#define ENGINE_CONTEXT_H

#include <queue>
#include <memory>
#include <map>
#include <set>
#include "core/Events.h"
#include "core/BasicBehaviors.hpp"

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
    private:
        ScreenManager() = default;
    };

    class EventManager
    {
    public:
        using Id = uint32_t;
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

    class ContextManager
    {
    public:
        using Id = uint32_t;
        ContextManager() = default;
        Context *load_context(const char *obj_file, EventManager &event_manager, ScreenManager &screen_manager);
        void unload_context(Id);
        virtual ~ContextManager();
    private:
        struct ContextInfo
        {
            void *handle;
            Context *context;
        };
        std::map<Id, ContextInfo> _contexts;
    };

    class Context : public behavior::UniqueId<uint32_t>
    {
        friend class EventManager;

    public:
        Context() = delete;
        Context(EventManager &event_manager, ScreenManager &screen_manager);
        virtual void evaluate();
        virtual void initialize();
        virtual ~Context();
    protected:
        virtual void subscribe(EventType t) final;
        virtual void unsubscribe(EventType t) final;
    private:
        using Item = typename EventManager::Item;
        using EventQueue = typename EventManager::EventQueue;
        void enqueue(Item event);
        void subscribe_impl(EventType t);
        void unsubscribe_impl(EventType t);
        void unsubscribe_impl();
        EventManager &_external_event_manager;
        ScreenManager &_screen_manager;
        EventQueue _event_queue;
    };
}

#endif //ENGINE_CONTEXT_H
