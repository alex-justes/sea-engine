#include "helpers/BasicContext.h"

extern "C"
{
    void* create_context(core::EventManager& event_manager, core::ScreenManager& screenManager)
    {
        return static_cast<void*>(new helpers::context::BasicContext(event_manager, screenManager));
    }
}