#ifndef ENGINE_TYPES_H
#define ENGINE_TYPES_H

#include <cstdint>
#include "helpers/Containers.hpp"

using Id = uint32_t;
using AABB = helpers::containers::AABB;
using Roi = helpers::containers::Rect2D<uint32_t>;
using Rect = helpers::containers::Rect2D<uint32_t>;
using Size = helpers::containers::Size2D<uint32_t>;
using Point = helpers::containers::Point2D<uint32_t>;

#endif //ENGINE_TYPES_H
