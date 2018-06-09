#include "core/Context.h"

extern "C"
{
    void* create_context(core::EventManager& event_manager, core::ScreenManager& screenManager)
    {
        return static_cast<void*>(new core::Context(event_manager, screenManager));
    }
}