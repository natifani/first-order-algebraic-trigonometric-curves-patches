#pragma once

#include <GL/glew.h>

namespace cagd
{
    class Color4
    {
    protected:
        GLfloat _data[4]; // (r, g, b, a)

    public:
        Color4()
        {
            _data[0] = _data[1] = _data[2] = 0.0;
            _data[3] = 1.0;
        }

        Color4(GLfloat r, GLfloat g, GLfloat b, GLfloat a = 1.0f)
        {
            _data[0] = r;
            _data[1] = g;
            _data[2] = b;
            _data[3] = a;
        }

        // homework: get components by value .-.
        inline GLfloat operator [](GLuint rhs) const
        {
            return _data[rhs];
        }

        inline GLfloat r() const
        {
            return _data[0];
        }

        inline GLfloat g() const
        {
            return _data[1];
        }

        inline GLfloat b() const
        {
            return _data[2];
        }

        inline GLfloat a() const
        {
            return _data[3];
        }

        // homework: get components by reference .-.
        inline GLfloat& operator [](GLuint rhs)
        {
           return _data[rhs];
        }

        inline GLfloat& r()
        {
            return _data[0];
        }

        inline GLfloat& g()
        {
            return _data[1];
        }

        inline GLfloat& b()
        {
            return _data[2];
        }

        inline GLfloat& a()
        {
            return _data[3];
        }
    };
}
