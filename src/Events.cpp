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

MouseEvent::MouseEvent(EventType type, const Point &position, Id context, Id camera)
: Event(type),
  _context_id(context),
  _camera_id(camera)
{
    set_position(position);
}

Id MouseEvent::context_id() const
{
    return _context_id;
}

Id MouseEvent::camera_id() const
{
    return _camera_id;
}

void MouseEvent::set_position(const Point &position)
{
    Position::set_position(position);
}

MouseMoveEvent::MouseMoveEvent(const Point &position, Id context, Id camera)
: MouseEvent(EventType::MouseMove, position, context, camera)
{
}

MouseClickEvent::MouseClickEvent(const Point &position, MouseClickEvent::Button button, MouseClickEvent::State state,
                                 Id context, Id camera)
: MouseEvent(EventType::MouseClick, position, context, camera),
  _button(button),
  _state(state)
{
}

MouseClickEvent::Button MouseClickEvent::button() const
{
    return _button;
}

MouseClickEvent::State MouseClickEvent::state() const
{
    return _state;
}

