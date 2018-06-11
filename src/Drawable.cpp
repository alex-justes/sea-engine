#include "core/Drawable.h"

using namespace core::drawable;

DrawableRect::DrawableRect(const Size &box_size, const RGBA &fill_color, const RGBA &border_color)
{
    set_box_size(box_size);
    set_fill_color(fill_color);
    set_border_color(border_color);
}

std::list<const Drawable *> CompoundDrawable::get_drawables() const
{
    return std::list<const Drawable *>();
}
AABB DrawableRect::bounding_box() const
{
    return AABB(Point(0,0), box_size());
}
