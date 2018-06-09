#ifndef ENGINE_EVENTS_H
#define ENGINE_EVENTS_H

namespace core
{
    enum class EventType
    {
        Mouse,
        Keyboard,
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

    class MouseEvent: public Event
    {
    public:
        MouseEvent();
    };

    class KeyboardEvent: public Event
    {
    public:
        enum class State
        {
            PRESSED,
            RELEASED,
        };
        KeyboardEvent(State state, int sym, int code);
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
