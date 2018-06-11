#ifndef ENGINE_BASICACTORS_H
#define ENGINE_BASICACTORS_H

namespace core::actor::basic
{
    class IActor
    {
    public:
        virtual ~IActor() = default;
    };

    class Evaluate: public virtual IActor
    {
    public:
        virtual bool evaluate(uint32_t time_elapsed) = 0;
    };

    class Update: public virtual IActor
    {
    public:
        virtual bool update() = 0;
    };

    class Initialize: public virtual IActor
    {
    public:
        virtual void initialize() = 0;
    };

}

#endif //ENGINE_BASICACTORS_H
