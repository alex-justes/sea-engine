#ifndef ENGINE_COMPLEXBEHAVIORS_HPP
#define ENGINE_COMPLEXBEHAVIORS_HPP

#include "core/BasicBehaviors.hpp"

namespace core::behavior::complex
{
    using namespace core::behavior;

    class Renderable:
            public virtual basic::Position,
            public virtual basic::Z_Order,
            public virtual basic::RenderShape<AABB>
    {
    public:
        using Drawable = core::drawable::Drawable;
        const Drawable *drawable() const;
        template <class T, class ... Types>
        T* set_drawable(Types &&... args);
    private:
        std::unique_ptr<Drawable> _drawable;
    };

    // =======================================
    template<class T, class... Types>
    T *Renderable::set_drawable(Types &&... args)
    {
        static_assert(std::is_base_of_v<Drawable , T>, "T should be derived from Drawable");
        auto ptr = new T(std::forward<Types>(args)...);
        _drawable.reset(ptr);
        return ptr;
    }
}

#endif //ENGINE_COMPLEXBEHAVIORS_HPP