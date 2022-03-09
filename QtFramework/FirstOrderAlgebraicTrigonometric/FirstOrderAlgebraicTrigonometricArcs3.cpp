#include "FirstOrderAlgebraicTrigonometricArcs3.h"
#include "BasisFunctions.h"

using namespace cagd;
using namespace std;

FirstOrderAlgebraicTrigonometricArc3::FirstOrderAlgebraicTrigonometricArc3(GLdouble n, GLdouble alpha) :
    LinearCombination3(0.0, alpha, n),
    _alpha(alpha),
    _n(n)
{

}

GLboolean FirstOrderAlgebraicTrigonometricArc3::BlendingFunctionValues(GLdouble u, RowMatrix<GLdouble> &values) const
{
    if (u < 0.0)
        return GL_FALSE;

    values.ResizeColumns(4);

    values[0] = F0(_alpha, u);
    values[1] = F1(_alpha, u);
    values[2] = F2(_alpha, u);
    values[3] = F3(_alpha, u);

    return GL_TRUE;
}

GLboolean FirstOrderAlgebraicTrigonometricArc3::CalculateDerivatives(GLuint max_order_of_derivatives, GLdouble u, Derivatives& d) const
{
    if (u < 0.0 && max_order_of_derivatives > 2)
        return GL_FALSE;

    d.ResizeRows(max_order_of_derivatives + 1);
    d.LoadNullVectors();

    RowMatrix<GLdouble> u_bv(4), d1_u_bv(4), d2_u_bv(4);

    if (max_order_of_derivatives >= 0)
    {
        BlendingFunctionValues(u, u_bv);
        for (GLuint i = 0; i < 4; ++i)
        {
            d[0] += _data[i] * u_bv[i];
        }
    }


    if (max_order_of_derivatives >= 1)
    {
        d1_u_bv[0] = d1F0(_alpha, u);
        d1_u_bv[1] = d1F1(_alpha, u);
        d1_u_bv[2] = d1F2(_alpha, u);
        d1_u_bv[3] = d1F3(_alpha, u);
        for (GLuint i = 0; i < 4; ++i)
        {
            d[1] += _data[i] * d1_u_bv[i];
        }
    }


    if (max_order_of_derivatives >= 2)
    {
        d2_u_bv[0] = d2F0(_alpha, u);
        d2_u_bv[1] = d2F1(_alpha, u);
        d2_u_bv[2] = d2F2(_alpha, u);
        d2_u_bv[3] = d2F3(_alpha, u);
        for (GLuint i = 0; i < 4; ++i)
        {
            d[2] += _data[i] * d2_u_bv[i];
        }
    }

    return GL_TRUE;
}

GLboolean FirstOrderAlgebraicTrigonometricArc3::SetAlpha(GLdouble alpha)
{
    _alpha = alpha;
    _u_max = _alpha;
    return GL_TRUE;
}

GLdouble FirstOrderAlgebraicTrigonometricArc3::GetAlpha() const
{
    return _alpha;
}
