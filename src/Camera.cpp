#include "core/Camera.h"

using namespace core;

Camera::List Camera::get_visible_objects() const
{
    return List();
}
const Roi &Camera::roi() const
{
    return _roi;
}
