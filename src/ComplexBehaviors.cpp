#include "core/ComplexBehaviors.hpp"


using namespace core::behavior::complex;

const core::drawable::Drawable *Renderable::drawable() const
{
    return _drawable.get();
}
