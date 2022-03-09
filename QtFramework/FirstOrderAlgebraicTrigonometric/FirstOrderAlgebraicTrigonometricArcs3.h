#ifndef FIRSTORDERALGEBRAICTRIGONOMETRICARCS3_H
#define FIRSTORDERALGEBRAICTRIGONOMETRICARCS3_H

#include <Core/Constants.h>
#include <Core/LinearCombination3.h>

namespace cagd
{
    class FirstOrderAlgebraicTrigonometricArc3: public LinearCombination3
    {
    private:
        GLdouble _alpha; // possible shape parameter (if does not exist, delete this line)
        GLdouble _n;
    public:
        // special/default constructor
        FirstOrderAlgebraicTrigonometricArc3(GLdouble n = 4, GLdouble alpha = PI / 2.0);

        // inherited pure virtual methods have to be redeclared and defined
        GLboolean BlendingFunctionValues(GLdouble u, RowMatrix<GLdouble> &values) const;
        GLboolean CalculateDerivatives(GLuint max_order_of_derivatives, GLdouble u, Derivatives& d) const;

        // project-dependent setters/getters, e.g.,

        // if exists, you will need to set/get the shape/tension parameter of the basis/blending functions
        GLboolean SetAlpha(GLdouble alpha);
        GLdouble GetAlpha() const;
    };
}

#endif // FIRSTORDERALGEBRAICTRIGONOMETRICARCS3_H
