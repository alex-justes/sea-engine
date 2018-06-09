#ifndef ENGINE_BASICACTORS_H
#define ENGINE_BASICACTORS_H

namespace core::actor
{
    class Actor
    {
    public:
        virtual ~Actor() = default;
        virtual bool act() = 0;
    };

}

#endif //ENGINE_BASICACTORS_H
