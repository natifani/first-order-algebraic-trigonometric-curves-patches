#include <cmath>
#include "BasisFunctions.h"
#include "../Core/Constants.h"

using namespace cagd;
using namespace std;

GLdouble cagd::F0(GLdouble alpha, GLdouble u)
{
    return F3(alpha, alpha - u);
}

GLdouble cagd::F1(GLdouble alpha, GLdouble u)
{
    return F2(alpha, alpha - u);
}

GLdouble cagd::F2(GLdouble alpha, GLdouble u)
{
    return ((alpha - u + sin(alpha - u) + sin(u) - sin(alpha) + u * cos(alpha) - alpha * cos(u)) * sin(alpha))
            / ((alpha - sin(alpha)) * (2 * sin(alpha) - alpha - alpha * cos(alpha)));
}

GLdouble cagd::F3(GLdouble alpha, GLdouble u)
{
    return (u - sin(u)) / (alpha - sin(alpha));
}

// first-order derivatives
GLdouble cagd::d1F0(GLdouble alpha, GLdouble u)
{
    return (-1) * d1F3(alpha, alpha - u);
}

GLdouble cagd::d1F1(GLdouble alpha, GLdouble u)
{
    return (-1) * d1F2(alpha, alpha - u);
}

GLdouble cagd::d1F2(GLdouble alpha, GLdouble u)
{
    return (sin(alpha) * (-cos(u - alpha) + alpha * sin(u) + cos(u) + cos(alpha) - 1))
            / ((alpha - sin(alpha)) * (2 * sin(alpha) - alpha * cos(alpha) - alpha));
}

GLdouble cagd::d1F3(GLdouble alpha, GLdouble u)
{
    return (1 - cos(u)) / (alpha - sin(alpha));
}

// second-order derivatives
GLdouble cagd::d2F0(GLdouble alpha, GLdouble u)
{
     return d2F3(alpha, alpha - u);
}

GLdouble cagd::d2F1(GLdouble alpha, GLdouble u)
{
    return d2F2(alpha, alpha - u);
}

GLdouble cagd::d2F2(GLdouble alpha, GLdouble u)
{
    return (sin(alpha) * (sin(u - alpha) - sin(u) + alpha * cos(u)))
            / ((alpha - sin(alpha)) * (2 * sin(alpha) - alpha * cos(alpha) - alpha));
}

GLdouble cagd::d2F3(GLdouble alpha, GLdouble u)
{
    return (-sin(u)) / (sin(alpha) - alpha);
}

