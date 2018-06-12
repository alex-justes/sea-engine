#include "core/ComplexBehaviors.hpp"


using namespace core::complex::behavior;

const core::drawable::Drawable *Renderable::drawable() const
{
    return _drawable.get();
}

void Renderable::set_drawable(std::unique_ptr<Renderable::Drawable> &&drawable)
{
    _drawable = std::move(drawable);
}

Renderable::Drawable *Renderable::get_drawable()
{
    return _drawable.get();
}
