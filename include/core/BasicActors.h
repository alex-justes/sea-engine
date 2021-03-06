#ifndef ENGINE_BASICACTORS_H
#define ENGINE_BASICACTORS_H

namespace core::basic::actor
{
    class IActor
    {
    public:
        virtual ~IActor() = default;
    };

    class Evaluate: public virtual IActor
    {
    public:
        virtual void evaluate(uint32_t time_elapsed) = 0;
    };

    class Update: public virtual IActor
    {
    public:
        virtual bool update(bool force) = 0;
    };

    class Initialize: public virtual IActor
    {
    public:
        virtual void initialize() = 0;
    };

    class Fade: public virtual IActor
    {
    public:
        virtual void fade(float percent) = 0;
    };

    class Die: public virtual IActor
    {
    public:
        virtual void die() = 0;
    };

}

#endif //ENGINE_BASICACTORS_H
