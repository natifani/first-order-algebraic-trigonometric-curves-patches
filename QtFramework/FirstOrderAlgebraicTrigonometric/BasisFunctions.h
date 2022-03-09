#ifndef BASISFUNCTIONS_H
#define BASISFUNCTIONS_H

#include <GL/glew.h>

namespace cagd
{
    // the variable alpha denotes the possible shape parameter of the basis functions

    // zeroth-order derivatives
    GLdouble F0(GLdouble alpha, GLdouble u);
    GLdouble F1(GLdouble alpha, GLdouble u);
    GLdouble F2(GLdouble alpha, GLdouble u);
    GLdouble F3(GLdouble alpha, GLdouble u);

    // first-order derivatives
    GLdouble d1F0(GLdouble alpha, GLdouble u);
    GLdouble d1F1(GLdouble alpha, GLdouble u);
    GLdouble d1F2(GLdouble alpha, GLdouble u);
    GLdouble d1F3(GLdouble alpha, GLdouble u);

    // second-order derivatives
    GLdouble d2F0(GLdouble alpha, GLdouble u);
    GLdouble d2F1(GLdouble alpha, GLdouble u);
    GLdouble d2F2(GLdouble alpha, GLdouble u);
    GLdouble d2F3(GLdouble alpha, GLdouble u);
}

#endif // BASISFUNCTIONS_H