#include "core/Drawable.h"

using namespace core::drawable;


std::list<const Drawable *> CompoundDrawable::get_drawables() const
{
    return std::list<const Drawable *>();
}
AABB DrawableRect::bounding_box() const
{
    return AABB(Point(0,0), box_size());
}
