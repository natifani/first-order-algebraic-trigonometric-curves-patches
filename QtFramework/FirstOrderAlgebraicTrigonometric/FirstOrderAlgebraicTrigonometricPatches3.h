#ifndef FIRSTORDERALGEBRAICTRIGONOMETRICPATCH3_H
#define FIRSTORDERALGEBRAICTRIGONOMETRICPATCH3_H

#include <Core/TensorProductSurfaces3.h>
#include <Core/Constants.h>
#include <FirstOrderAlgebraicTrigonometric/BasisFunctions.h>

namespace cagd
{
    enum Variable{U = 0, V = 1};

    class FirstOrderAlgebraicTrigonometricPatch3: public TensorProductSurface3
    {
    private:
        GLdouble _alpha[2]; // _alpha[U], _alpha[V], i.e., possible shape parameters in directions u and v, respectively

    public:
        FirstOrderAlgebraicTrigonometricPatch3(GLdouble u_alpha = PI / 2.0, GLdouble v_alpha = PI / 2.0);

        // inherited pure virtual methods have to be redeclared and defined
        GLboolean UBlendingFunctionValues(GLdouble u, RowMatrix<GLdouble> &blending_values) const;
        GLboolean VBlendingFunctionValues(GLdouble v, RowMatrix<GLdouble> &blending_values) const;
        GLboolean CalculatePartialDerivatives(
        GLuint maximum_order_of_partial_derivatives,
        GLdouble u, GLdouble v, PartialDerivatives &pd) const;

        // alpha set-get
        void setAlphaU(GLdouble alpha);
        void setAlphaV(GLdouble alpha);
        GLdouble getAlphaU();
        GLdouble getAlphaV();
    };
}

#endif // FIRSTORDERALGEBRAICTRIGONOMETRICPATCH3_H
