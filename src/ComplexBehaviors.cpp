#include "core/ComplexBehaviors.hpp"


using namespace core::complex::behavior;

const core::drawable::Drawable *Renderable::drawable() const
{
    return _drawable.get();
}
