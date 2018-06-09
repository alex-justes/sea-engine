#ifndef ENGINE_BASICCONTEXT_H
#define ENGINE_BASICCONTEXT_H

#include "core/Context.h"

namespace helpers::context
{
    class BasicContext : public core::Context
    {
    public:
        BasicContext() = delete;
        BasicContext(core::EventManager &event_manager, core::ScreenManager &screen_manager);
        virtual void evaluate() override;
        virtual void initialize() override;
    };
}

#endif //ENGINE_BASICCONTEXT_H
