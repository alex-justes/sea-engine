#ifndef ENGINE_EVENTS_H
#define ENGINE_EVENTS_H

#include "BasicBehaviors.hpp"

namespace core
{
    enum class EventType
    {
        MouseMove,
        MouseClick,
        KeyPress,
    };

    class Event
    {
    public:
        Event() = delete;
        explicit Event(EventType e);
        virtual ~Event() = default;
        EventType type() const;
    private:
        const EventType _type;
    };

    class MouseEvent :
            public Event,
            public core::basic::behavior::Position
    {
    public:
        Id context_id() const;
        Id camera_id() const;
    protected:
        MouseEvent(EventType type, const Point &position, Id context, Id camera);
        void set_position(const Point &position) override;
    private:
        const Id _context_id;
        const Id _camera_id;
    };

    class MouseClickEvent :
            public MouseEvent
    {
    public:
        enum class State
        {
            PRESSED,
            RELEASED,
        };
        enum class Button
        {
            LEFT,
            RIGHT,
        };
        MouseClickEvent(const Point &position, Button button, State state, Id context, Id camera);
        Button button() const;
        State state() const;
    private:
        const Button _button;
        const State _state;
    };

    class MouseMoveEvent :
            public MouseEvent
    {
    public:
        MouseMoveEvent(const Point &position, Id context, Id camera);
    };


    class KeyPressEvent : public Event
    {
    public:
        enum class State
        {
            PRESSED,
            RELEASED,
        };
        KeyPressEvent(State state, int sym, int code);
        int sym() const;
        int code() const;
        State state() const;
    private:
        const int _sym;
        const int _code;
        const State _state;
    };
}

#endif //ENGINE_EVENTS_H
