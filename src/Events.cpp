#include "core/Events.h"

using namespace core;

Event::Event(EventType e)
: _type(e)
{
}

EventType Event::type() const
{
    return _type;
}

MouseEvent::MouseEvent()
: Event(EventType::Mouse)
{

}

KeyboardEvent::KeyboardEvent(State state, int sym, int code)
: Event(EventType::Keyboard),
  _state(state),
  _sym(sym),
  _code(code)
{

}
int KeyboardEvent::sym() const
{
    return _sym;
}
int KeyboardEvent::code() const
{
    return _code;
}
KeyboardEvent::State KeyboardEvent::state() const
{
    return _state;
}