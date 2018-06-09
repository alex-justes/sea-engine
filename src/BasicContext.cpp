#include "helpers/BasicContext.h"
#include "core/Events.h"

using namespace helpers::context;

BasicContext::BasicContext(core::EventManager &event_manager, core::ScreenManager &screen_manager)
        :
        Context(event_manager, screen_manager)
{

}
void BasicContext::evaluate()
{
    Item event;
    while (events_pop(event))
    {
        LOG_S("Event!")
    }
}
void BasicContext::initialize()
{
    subscribe(core::EventType::Mouse);
    subscribe(core::EventType::Keyboard);
}

