#ifndef ENGINE_BASICACTORS_H
#define ENGINE_BASICACTORS_H

namespace core::actor
{
    class Actor
    {
    public:
        virtual ~Actor() = default;
        virtual bool act(uint32_t time_elapsed) = 0;
    };

}

#endif //ENGINE_BASICACTORS_H
