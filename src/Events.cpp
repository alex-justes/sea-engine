#include "core/Events.h"

using namespace core;

Event::Event(EventType e)
        :
        _type(e)
{
}

EventType Event::type() const
{
    return _type;
}


KeyPressEvent::KeyPressEvent(State state, int sym, int code)
        :
        Event(EventType::KeyPress),
        _state(state),
        _sym(sym),
        _code(code)
{

}
int KeyPressEvent::sym() const
{
    return _sym;
}
int KeyPressEvent::code() const
{
    return _code;
}
KeyPressEvent::State KeyPressEvent::state() const
{
    return _state;
}

MouseMoveEvent::MouseMoveEvent(const Point &position, Id context, Id camera)
        :
        Event(EventType::MouseMove),
        _context_id(context),
        _camera_id(camera)
{
    set_position(position);
}

void MouseMoveEvent::set_position(const Point &position)
{
    Position::set_position(position);
}

Id MouseMoveEvent::context_id() const
{
    return _context_id;
}

Id MouseMoveEvent::camera_id() const
{
    return _camera_id;
}

MouseClickEvent::MouseClickEvent(const Point &position, MouseClickEvent::Button button, MouseClickEvent::State state,
                                 Id context, Id camera)
: Event(EventType::MouseClick),
  _button(button),
  _state(state),
  _context_id(context),
  _camera_id(camera)
{
    set_position(position);
}

MouseClickEvent::Button MouseClickEvent::button() const
{
    return _button;
}

MouseClickEvent::State MouseClickEvent::state() const
{
    return _state;
}

Id MouseClickEvent::context_id() const
{
    return _context_id;
}

Id MouseClickEvent::camera_id() const
{
    return _camera_id;
}

void MouseClickEvent::set_position(const Point &position)
{
    Position::set_position(position);
}
