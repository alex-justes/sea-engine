#include "core/AffineTransformation.h"

#include <algorithm>

using namespace core::transformation;

AffineTransformation::AffineTransformation()
{
    _affine.create({2, 3});
    std::fill(_affine.data_begin(), _affine.data_end(), 0);
    _affine[0][0] = 1.f;
    _affine[1][1] = 1.f;
}

AffineTransformation::AffineTransformation(const AffineTransformation &t)
{
    *this = t;
}

AffineTransformation &AffineTransformation::operator=(const AffineTransformation &t)
{
    if (this != &t)
    {
        std::copy(t._affine.data_begin(), t._affine.data_end(), _affine.data_begin());
    }
    return *this;
}

void AffineTransformation::scale(const PointF &scale)
{
    _affine[0][0] *= scale.x;
    _affine[0][1] *= scale.y;
    _affine[1][0] *= scale.x;
    _affine[1][1] *= scale.y;
}

void AffineTransformation::shift(const PointF &shift)
{
    _affine[0][2] += shift.x;
    _affine[1][2] += shift.y;
}

void AffineTransformation::rotate_deg(float a)
{
    rotate((float) M_PI * a / 180.f);
}

void AffineTransformation::rotate(float angle)
{
    float a = _affine[0][0];
    float b = _affine[0][1];
    float d = _affine[1][0];
    float e = _affine[1][1];

    _affine[0][0] = std::cos(angle) * a + std::sin(angle) * b;
    _affine[0][1] = -std::sin(angle) * a + std::cos(angle) * b;
    _affine[1][0] = std::cos(angle) * d + std::sin(angle) * e;
    _affine[1][1] = -std::sin(angle) * d + std::cos(angle) * e;
}

AffineTransformation &AffineTransformation::invert()
{
    float a = _affine[0][0];
    float b = _affine[0][1];
    float c = _affine[0][2];
    float d = _affine[1][0];
    float e = _affine[1][1];
    float f = _affine[1][2];

    float divisor = a * e - b * d;

    if (divisor > -1.0e-8 && divisor < 1.0e-8)
    {
        std::fill(_affine.data_begin(), _affine.data_end(), 0);
        _affine[0][0] = 1.f;
        _affine[1][1] = 1.f;
        return *this;
    }
    _affine[0][0] = e / divisor;
    _affine[0][1] = -b / divisor;
    _affine[0][2] = (b * f - e * c) / divisor;
    _affine[1][0] = -d / divisor;
    _affine[1][1] = a / divisor;
    _affine[1][2] = (c * d - a * f) / divisor;
    return *this;
}

AffineTransformation AffineTransformation::inverse() const
{
    AffineTransformation res;
    float a = _affine[0][0];
    float b = _affine[0][1];
    float c = _affine[0][2];
    float d = _affine[1][0];
    float e = _affine[1][1];
    float f = _affine[1][2];

    float divisor = a * e - b * d;

    if (divisor > -1.0e-8 && divisor < 1.0e-8)
    {
        std::fill(res._affine.data_begin(), res._affine.data_end(), 0);
        res._affine[0][0] = 1.f;
        res._affine[1][1] = 1.f;
        return res;
    }
    res._affine[0][0] = e / divisor;
    res._affine[0][1] = -b / divisor;
    res._affine[0][2] = (b * f - e * c) / divisor;
    res._affine[1][0] = -d / divisor;
    res._affine[1][1] = a / divisor;
    res._affine[1][2] = (c * d - a * f) / divisor;
    return res;
}

PointF AffineTransformation::offset() const
{
    return {_affine[0][2], _affine[1][2]};
}

PointF AffineTransformation::scale() const
{
    PointF sx{_affine[0][0], _affine[1][0]};
    PointF sy{_affine[0][1], _affine[1][1]};
    return {sx.length(), sy.length()};
}

