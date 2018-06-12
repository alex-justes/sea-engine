#ifndef ENGINE_AFFINETRANSFORMATION_H
#define ENGINE_AFFINETRANSFORMATION_H

#include "helpers/Containers.hpp"
#include "core/Types.h"

//PointF operator*(const core::transformation::AffineTransformation& mat, const PointF& pt);
//core::transformation::AffineTransformation operator*(const core::transformation::AffineTransformation& lhs, const core::transformation::AffineTransformation& rhs);

namespace core::transformation
{
    class AffineTransformation
    {
    public:
        using Matrix = helpers::containers::Matrix<float, 2>;
        AffineTransformation();
        AffineTransformation(const AffineTransformation &t);
        AffineTransformation &operator=(const AffineTransformation &t);
        void scale(const PointF &scale);
        void shift(const PointF &shift);
        void rotate_deg(float a);
        void rotate(float a);
        AffineTransformation &invert();
        AffineTransformation inverse() const;

        PointF offset() const;
        PointF scale() const;

        friend PointF operator*(const core::transformation::AffineTransformation &mat, const PointF &pt)
        {
            const auto &affine = mat._affine;
            return
                    {
                            pt.x * affine[0][0] + pt.y * affine[0][1] + affine[0][2],
                            pt.x * affine[1][0] + pt.y * affine[1][1] + affine[1][2]
                    };
        }

        friend AffineTransformation operator*(const AffineTransformation &lhs, const AffineTransformation &rhs)
        {
            AffineTransformation res;
            float a = lhs._affine[0][0];
            float b = lhs._affine[0][1];
            float c = lhs._affine[0][2];
            float d = lhs._affine[1][0];
            float e = lhs._affine[1][1];
            float f = lhs._affine[1][2];

            float _a = rhs._affine[0][0];
            float _b = rhs._affine[0][1];
            float _c = rhs._affine[0][2];
            float _d = rhs._affine[1][0];
            float _e = rhs._affine[1][1];
            float _f = rhs._affine[1][2];

            res._affine[0][0] = (a * _a + b * _d);
            res._affine[0][1] = (a * _b + b * _e);
            res._affine[0][2] = (a * _c + b * _f + c);
            res._affine[1][0] = (d * _a + e * _d);
            res._affine[1][1] = (d * _b + e * _e);
            res._affine[1][2] = (d * _c + e * _f + f);

            return res;
        }

    private:
        Matrix _affine;
    };

    // =====================================================
}

//PointF operator*(const core::transformation::AffineTransformation& mat, const PointF& pt);

#endif //ENGINE_AFFINETRANSFORMATION_H
