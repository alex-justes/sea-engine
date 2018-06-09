#ifndef ENGINE_CAMERA_H
#define ENGINE_CAMERA_H

#include <list>
#include "core/BasicBehaviors.hpp"
#include "core/Types.h"

namespace core
{
    class Camera: public behavior::Position
    {
    public:
        using List = std::list<const behavior::Renderable*>;
        List get_visible_objects() const;
        const Roi& roi() const;
    private:
        Roi _roi;
    };
}

#endif //ENGINE_CAMERA_H
