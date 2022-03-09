#include <FirstOrderAlgebraicTrigonometric/FirstOrderAlgebraicTrigonometricPatches3.h>

using namespace cagd;

FirstOrderAlgebraicTrigonometricPatch3::FirstOrderAlgebraicTrigonometricPatch3(GLdouble u_alpha, GLdouble v_alpha) :
    TensorProductSurface3(0, u_alpha, 0, v_alpha, 4, 4)

{
    _alpha[U] = u_alpha;
    _alpha[V] = v_alpha;
}

// inherited pure virtual methods have to be redeclared and defined
GLboolean FirstOrderAlgebraicTrigonometricPatch3::UBlendingFunctionValues(GLdouble u, RowMatrix<GLdouble> &blending_values) const
{
    assert(u >= 0 && u <= _alpha[U]);

    blending_values.ResizeColumns(4);

    blending_values(0) = F0(_alpha[U], u);
    blending_values(1) = F1(_alpha[U], u);
    blending_values(2) = F2(_alpha[U], u);
    blending_values(3) = F3(_alpha[U], u);

    return true;
}

GLboolean FirstOrderAlgebraicTrigonometricPatch3::VBlendingFunctionValues(GLdouble v, RowMatrix<GLdouble> &blending_values) const
{
    assert(v >= 0 && v <= _alpha[V]);

    blending_values.ResizeColumns(4);

    blending_values(0) = F0(_alpha[V], v);
    blending_values(1) = F1(_alpha[V], v);
    blending_values(2) = F2(_alpha[V], v);
    blending_values(3) = F3(_alpha[V], v);

    return true;
}

GLboolean FirstOrderAlgebraicTrigonometricPatch3::CalculatePartialDerivatives(
                GLuint maximum_order_of_partial_derivatives,
                GLdouble u, GLdouble v, PartialDerivatives &pd) const
{
    assert(u >= 0 && u <= _alpha[U] && v >= 0 && v <= _alpha[V] && maximum_order_of_partial_derivatives >= 1);

    pd.ResizeRows(3);
    pd.LoadNullVectors();

    RowMatrix<GLdouble> u_blending_values, v_blending_values;
    UBlendingFunctionValues(u, u_blending_values);
    VBlendingFunctionValues(v, v_blending_values);

    RowMatrix<GLdouble> u_d1_values(4);
    u_d1_values(0) = d1F0(_alpha[U], u);
    u_d1_values(1) = d1F1(_alpha[U], u);
    u_d1_values(2) = d1F2(_alpha[U], u);
    u_d1_values(3) = d1F3(_alpha[U], u);

    RowMatrix<GLdouble> u_d2_values(4);
    u_d2_values(0) = d2F0(_alpha[U], u);
    u_d2_values(1) = d2F1(_alpha[U], u);
    u_d2_values(2) = d2F2(_alpha[U], u);
    u_d2_values(3) = d2F3(_alpha[U], u);

    RowMatrix<GLdouble> v_d1_values(4);
    v_d1_values(0) = d1F0(_alpha[V], v);
    v_d1_values(1) = d1F1(_alpha[V], v);
    v_d1_values(2) = d1F2(_alpha[V], v);
    v_d1_values(3) = d1F3(_alpha[V], v);

    RowMatrix<GLdouble> v_d2_values(4);
    v_d2_values(0) = d2F0(_alpha[V], v);
    v_d2_values(1) = d2F1(_alpha[V], v);
    v_d2_values(2) = d2F2(_alpha[V], v);
    v_d2_values(3) = d2F3(_alpha[V], v);

    for (GLuint i = 0; i < 4; i++)
    {
        DCoordinate3 aux_d0_v, aux_d1_v, aux_d2_v;
        for (GLuint j = 0; j < 4; j++)
        {
            aux_d0_v += _data(i, j) * v_blending_values(j);
            aux_d1_v += _data(i, j) * v_d1_values(j);
            aux_d2_v += _data(i, j) * v_d2_values(j);
        }

        pd(0, 0) += aux_d0_v * u_blending_values(i);
        pd(1, 0) += aux_d0_v * u_d1_values(i);
        pd(1, 1) += aux_d1_v * u_blending_values(i);
        pd(2, 0) += aux_d0_v * u_d2_values(i);
        pd(2, 1) += aux_d1_v * u_d1_values(i);
        pd(2, 2) += aux_d2_v * u_blending_values(i);
    }

    return true;
}

void FirstOrderAlgebraicTrigonometricPatch3::setAlphaU(GLdouble alpha)
{
    _alpha[U] = alpha;
    _u_max = alpha;
}

void FirstOrderAlgebraicTrigonometricPatch3::setAlphaV(GLdouble alpha)
{
    _alpha[V] = alpha;
    _v_max = alpha;
}

GLdouble FirstOrderAlgebraicTrigonometricPatch3::getAlphaU()
{
    return _alpha[U];
}

GLdouble FirstOrderAlgebraicTrigonometricPatch3::getAlphaV()
{
    return _alpha[V];
}
