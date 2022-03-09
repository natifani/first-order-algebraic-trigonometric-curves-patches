#include "FirstOrderAlgebraicTrigonometricCompositeCurves3.h"
#include <iostream>
#include <random>

using namespace cagd;

FirstOrderAlgebraicTrigonometricCompositeCurve3::FirstOrderAlgebraicTrigonometricCompositeCurve3(GLdouble alpha, GLuint minimal_arc_count_to_be_reserved)
{
    _alpha = alpha;
    _attributes.resize(minimal_arc_count_to_be_reserved);
}

GLboolean FirstOrderAlgebraicTrigonometricCompositeCurve3::InsertNewArc(Color4 *color, GLenum usage_flag)
{
    GLuint index = _arc_count;
    _attributes[index].arc = new (nothrow) FirstOrderAlgebraicTrigonometricArc3();
    _attributes[index].color = color;

    if(!_attributes[index].arc)
    {
        DeleteExistingArc(index);
        return false;
    }

    for (GLuint i = 0; i < 4; i++)
    {
        GLdouble u = i * TWO_PI / 4;
        DCoordinate3 &cp = (*_attributes[index].arc)[i];

        cp[0] = cos(u);
        cp[1] = sin(u);
    }

    _attributes[index].arc->UpdateVertexBufferObjectsOfData();


    _attributes[index].image = _attributes[index].arc->GenerateImage(_arc_max_order_of_derivatives, _arc_div_point_count, usage_flag);

    if (!_attributes[index].image)
    {
         DeleteExistingArc(index);
         return GL_FALSE;
    }

    _attributes[index].image->UpdateVertexBufferObjects();

    _arc_count++;

    return GL_TRUE;
}

GLboolean FirstOrderAlgebraicTrigonometricCompositeCurve3::DeleteExistingArc(GLuint index)
{
    if (_attributes[index].arc)
    {
        if (_attributes[index].next)
        {
            if (_attributes[index].next_connection_type == LEFT)
            {
                _attributes[index].next->previous = nullptr;
            }
            else
            {
                _attributes[index].next->next = nullptr;
            }

        }

        if (_attributes[index].previous)
        {
            if (_attributes[index].previous_connection_type == LEFT)
            {
                _attributes[index].previous->previous = nullptr;
            }
            else
            {
                _attributes[index].previous->next = nullptr;
            }

        }

        delete _attributes[index].arc; _attributes[index].arc = nullptr;

    }

    if (_attributes[index].image)
    {
        delete _attributes[index].image; _attributes[index].image = nullptr;
    }

    return GL_TRUE;
}


GLboolean FirstOrderAlgebraicTrigonometricCompositeCurve3::GenerateImageOfSelectedArcs(GLuint index_0, GLuint index_1)
{
    if (_attributes[index_0].arc == nullptr || _attributes[index_1].arc == nullptr)
    {
        return GL_FALSE;
    }

    delete _attributes[index_0].image; _attributes[index_0].image = nullptr;

    _attributes[index_0].image = _attributes[index_0].arc->GenerateImage(_arc_max_order_of_derivatives, _arc_div_point_count);

    if (!_attributes[index_0].image)
    {
        return GL_FALSE;
    }

    if (!_attributes[index_0].image->UpdateVertexBufferObjects())
    {
        return GL_FALSE;
    }

    if (!_attributes[index_0].arc->UpdateVertexBufferObjectsOfData())
    {
        return GL_FALSE;
    }

    delete _attributes[index_1].image; _attributes[index_1].image = nullptr;

    _attributes[index_1].image = _attributes[index_1].arc->GenerateImage(_arc_max_order_of_derivatives, _arc_div_point_count);

    if (!_attributes[index_1].image)
    {
        return GL_FALSE;
    }

    if (!_attributes[index_1].image->UpdateVertexBufferObjects())
    {
        return GL_FALSE;
    }

    if (!_attributes[index_1].arc->UpdateVertexBufferObjectsOfData())
    {
        return GL_FALSE;
    }

    return GL_TRUE;
}


GLboolean FirstOrderAlgebraicTrigonometricCompositeCurve3::ArcExists(GLuint index) const
{
    if (_attributes[index].arc == nullptr)
    {
        return GL_FALSE;
    }

    return GL_TRUE;
}

GLboolean FirstOrderAlgebraicTrigonometricCompositeCurve3::MergeExistingArcs(GLuint index_0, Direction direction_0, GLuint index_1, Direction direction_1)
{
    assert((direction_0 == RIGHT || direction_0 == LEFT) && (direction_1 == RIGHT || direction_1 == LEFT));

    DCoordinate3 centre;
    if (direction_0 == RIGHT && direction_1 == LEFT && !_attributes[index_0].next && !_attributes[index_1].previous)
    {
        centre = ((*_attributes[index_0].arc)[1] + (*_attributes[index_1].arc)[2]) / 2;
        (*_attributes[index_0].arc)[0] = centre;
        (*_attributes[index_1].arc)[3] = centre;
        _attributes[index_0].next = &_attributes[index_1];
        _attributes[index_0].next_connection_type = LEFT;
        _attributes[index_1].previous = &_attributes[index_0];
        _attributes[index_1].previous_connection_type = RIGHT;

        return GenerateImageOfSelectedArcs(index_0, index_1);
    }

    if (direction_0 == RIGHT && direction_1 == RIGHT && !_attributes[index_0].next && !_attributes[index_1].next)
    {
        centre = ((*_attributes[index_0].arc)[1] + (*_attributes[index_1].arc)[1]) / 2;
        (*_attributes[index_0].arc)[0] = centre;
        (*_attributes[index_1].arc)[0] = centre;
        _attributes[index_0].next = &_attributes[index_1];
        _attributes[index_0].next_connection_type = RIGHT;
        _attributes[index_1].next = &_attributes[index_0];
        _attributes[index_1].next_connection_type = RIGHT;
        return GenerateImageOfSelectedArcs(index_0, index_1);
    }

    if (direction_0 == LEFT && direction_1 == LEFT && !_attributes[index_0].previous && !_attributes[index_1].previous)
    {
        centre = ((*_attributes[index_0].arc)[2] + (*_attributes[index_1].arc)[2]) / 2;
        (*_attributes[index_0].arc)[3] = centre;
        (*_attributes[index_1].arc)[3] = centre;
        _attributes[index_0].previous = &_attributes[index_1];
        _attributes[index_0].previous_connection_type = LEFT;
        _attributes[index_1].previous = &_attributes[index_0];
        _attributes[index_1].previous_connection_type = LEFT;
        return GenerateImageOfSelectedArcs(index_0, index_1);
    }

    if (direction_0 == LEFT && direction_1 == RIGHT && !_attributes[index_0].previous && !_attributes[index_1].next)
    {
        centre = ((*_attributes[index_0].arc)[2] + (*_attributes[index_1].arc)[1]) / 2;
        (*_attributes[index_0].arc)[3] = centre;
        (*_attributes[index_1].arc)[0] = centre;
        _attributes[index_0].previous = &_attributes[index_1];
        _attributes[index_0].previous_connection_type = LEFT;
        _attributes[index_1].next = &_attributes[index_0];
        _attributes[index_1].next_connection_type = RIGHT;
        return GenerateImageOfSelectedArcs(index_0, index_1);
    }

    return GL_FALSE;
}
void FirstOrderAlgebraicTrigonometricCompositeCurve3::DeleteAllArcs()
{
    for (GLuint i = 0; i < _arc_count; i++)
    {
        if (ArcExists(i))
        {
            DeleteExistingArc(i);
        }

    }
}

GLboolean FirstOrderAlgebraicTrigonometricCompositeCurve3::RenderAllArcs(GLuint order, GLenum render_mode) const
{
    for (GLuint i = 0; i < _arc_count; i++)
    {
        if (ArcExists(i))
        {
            RenderSelectedArc(i, order, render_mode);
        }
    }

    return GL_TRUE;
}

GLboolean FirstOrderAlgebraicTrigonometricCompositeCurve3::RenderSelectedArc(GLuint index, GLuint order, GLenum render_mode) const
{
    assert(index >= 0 && index <= _attributes.size());

    if (!_attributes[index].arc)
    {
         return GL_FALSE;
    }

    if (_attributes[index].arc && render_mode == GL_POINTS)
    {
        glPointSize(15.0f);
        glBegin(GL_POINTS);
            DCoordinate3 cp;
            glColor3f(0.0f, 1.0f, 0.0f);
            cp = (*_attributes[index].arc)[0];
            glVertex3f(cp[0], cp[1], cp[2]);

            glColor3f(1.0f, 1.0f, 1.0f);
            cp = (*_attributes[index].arc)[1];
            glVertex3f(cp[0], cp[1], cp[2]);

            glColor3f(1.0f, 1.0f, 1.0f);
            cp = (*_attributes[index].arc)[2];
            glVertex3f(cp[0], cp[1], cp[2]);

            glColor3f(1.0f, 0.0f, 0.0f);
            cp = (*_attributes[index].arc)[3];
            glVertex3f(cp[0], cp[1], cp[2]);
        glEnd();
        glPointSize(1.0f);

        if (index != _selected_arc_index)
        {
            glColor3f(1.0f, 1.0f, 1.0f);
        }
        else
        {
            glColor3f(1.0f, 1.0f, 0.0f);
        }
        _attributes[index].arc->RenderData(GL_LINE_STRIP);

    }

    if (_attributes[index].image && render_mode != GL_POINTS)
    {
        glColor3f(_attributes[index].color->r(), _attributes[index].color->g(), _attributes[index].color->b());
        _attributes[index].image->RenderDerivatives(order, render_mode);
        return GL_TRUE;
    }

    return GL_FALSE;
}

void FirstOrderAlgebraicTrigonometricCompositeCurve3::RenderAllArcsScale()
{
    for (GLuint i = 0; i < _arc_count; i++)
    {
        _attributes[i].image->UpdateVertexBufferObjects(_arc_scale);
    }
}

void FirstOrderAlgebraicTrigonometricCompositeCurve3::ModifyPositionOfSelectedArc(GLuint index, GLuint cp_index, double x, double y, double z, GLenum usage_flag)
{
    try
    {
        if (_attributes[index].arc)
        {
            if ((_attributes[index].next == nullptr && _attributes[index].previous == nullptr) || (cp_index == 0 && _attributes[index].next == nullptr) ||
                    (cp_index == 1 && _attributes[index].next == nullptr) || (cp_index == 2 && _attributes[index].previous == nullptr) || (cp_index == 3 && _attributes[index].previous == nullptr))
            {
                (*_attributes[index].arc)[cp_index][0] = x;
                (*_attributes[index].arc)[cp_index][1] = y;
                (*_attributes[index].arc)[cp_index][2] = z;

                delete _attributes[index].image; _attributes[index].image = nullptr;
                _attributes[index].image = _attributes[index].arc->GenerateImage(_arc_max_order_of_derivatives, _arc_div_point_count, usage_flag);

                if (!_attributes[index].image)
                {
                    DeleteAllArcs();
                    throw Exception("Error occured during the image creation of the arc!");

                }

                if (!_attributes[index].image->UpdateVertexBufferObjects(_arc_scale))
                {
                    DeleteAllArcs();
                    throw Exception("Error occured during the update of the VBOs of the arc!");
                }

                if (!_attributes[index].arc->UpdateVertexBufferObjectsOfData())
                {
                    DeleteAllArcs();
                    throw Exception("Error occured during the update of the VBOs of the arc!");
                }

                return;
            }

            if (cp_index == 0 && _attributes[index].next)
            {
                Direction next_connection = _attributes[index].next_connection_type;

                if (next_connection == LEFT)
                {
                    (*_attributes[index].arc)[cp_index][0] = x;
                    (*_attributes[index].arc)[cp_index][1] = y;
                    (*_attributes[index].arc)[cp_index][2] = z;

                    (*_attributes[index].next->arc)[3] = (*_attributes[index].arc)[cp_index];

                    (*_attributes[index].next->arc)[2] = 2 * (*_attributes[index].arc)[cp_index] - (*_attributes[index].arc)[1];


                }

                if (next_connection == RIGHT)
                {
                    (*_attributes[index].arc)[cp_index][0] = x;
                    (*_attributes[index].arc)[cp_index][1] = y;
                    (*_attributes[index].arc)[cp_index][2] = z;

                    (*_attributes[index].next->arc)[0] = (*_attributes[index].arc)[cp_index];

                    (*_attributes[index].next->arc)[1] = 2 * (*_attributes[index].arc)[cp_index] - (*_attributes[index].arc)[1];

                }

                delete _attributes[index].image; _attributes[index].image = nullptr;
                _attributes[index].image = _attributes[index].arc->GenerateImage(_arc_max_order_of_derivatives, _arc_div_point_count, usage_flag);

                if (!_attributes[index].image)
                {
                    DeleteAllArcs();
                    throw Exception("Error occured during the image creation of the arc!");

                }

                if (!_attributes[index].image->UpdateVertexBufferObjects(_arc_scale))
                {
                    DeleteAllArcs();
                    throw Exception("Error occured during the update of the VBOs of the arc!");
                }

                if (!_attributes[index].arc->UpdateVertexBufferObjectsOfData())
                {
                    DeleteAllArcs();
                    throw Exception("Error occured during the update of the VBOs of the arc!");
                }

                delete _attributes[index].next->image; _attributes[index].next->image = nullptr;
                _attributes[index].next->image = _attributes[index].next->arc->GenerateImage(_arc_max_order_of_derivatives, _arc_div_point_count, usage_flag);

                if (!_attributes[index].next->image)
                {
                    DeleteAllArcs();
                    throw Exception("Error occured during the image creation of the arc!");

                }

                if (!_attributes[index].next->image->UpdateVertexBufferObjects(_arc_scale))
                {
                    DeleteAllArcs();
                    throw Exception("Error occured during the update of the VBOs of the arc!");
                }

                if (!_attributes[index].next->arc->UpdateVertexBufferObjectsOfData())
                {
                    DeleteAllArcs();
                    throw Exception("Error occured during the update of the VBOs of the arc!");
                }
            }

            if (cp_index == 3 && _attributes[index].previous)
            {
                Direction prev_connection = _attributes[index].previous_connection_type;

                if (prev_connection == LEFT)
                {
                    (*_attributes[index].arc)[cp_index][0] = x;
                    (*_attributes[index].arc)[cp_index][1] = y;
                    (*_attributes[index].arc)[cp_index][2] = z;

                    (*_attributes[index].previous->arc)[3] = (*_attributes[index].arc)[cp_index];

                    (*_attributes[index].previous->arc)[2] = 2 * (*_attributes[index].arc)[cp_index] - (*_attributes[index].arc)[2];



                }

                if (prev_connection == RIGHT)
                {
                    (*_attributes[index].arc)[cp_index][0] = x;
                    (*_attributes[index].arc)[cp_index][1] = y;
                    (*_attributes[index].arc)[cp_index][2] = z;

                    (*_attributes[index].previous->arc)[0] = (*_attributes[index].arc)[cp_index];

                    (*_attributes[index].previous->arc)[1] = 2 * (*_attributes[index].arc)[cp_index] - (*_attributes[index].arc)[2];

                }

                delete _attributes[index].image; _attributes[index].image = nullptr;
                _attributes[index].image = _attributes[index].arc->GenerateImage(_arc_max_order_of_derivatives, _arc_div_point_count, usage_flag);

                if (!_attributes[index].image)
                {
                    DeleteAllArcs();
                    throw Exception("Error occured during the image creation of the arc!");

                }

                if (!_attributes[index].image->UpdateVertexBufferObjects(_arc_scale))
                {
                    DeleteAllArcs();
                    throw Exception("Error occured during the update of the VBOs of the arc!");
                }

                if (!_attributes[index].arc->UpdateVertexBufferObjectsOfData())
                {
                    DeleteAllArcs();
                    throw Exception("Error occured during the update of the VBOs of the arc!");
                }

                delete _attributes[index].previous->image; _attributes[index].previous->image = nullptr;
                _attributes[index].previous->image = _attributes[index].previous->arc->GenerateImage(_arc_max_order_of_derivatives, _arc_div_point_count, usage_flag);

                if (!_attributes[index].previous->image)
                {
                    DeleteAllArcs();
                    throw Exception("Error occured during the image creation of the arc!");

                }

                if (!_attributes[index].previous->image->UpdateVertexBufferObjects(_arc_scale))
                {
                    DeleteAllArcs();
                    throw Exception("Error occured during the update of the VBOs of the arc!");
                }

                if (!_attributes[index].previous->arc->UpdateVertexBufferObjectsOfData())
                {
                    DeleteAllArcs();
                    throw Exception("Error occured during the update of the VBOs of the arc!");
                }
            }

            if (cp_index == 1 && _attributes[index].next)
            {
                Direction next_connection = _attributes[index].next_connection_type;

                if (next_connection == LEFT)
                {
                    (*_attributes[index].arc)[cp_index][0] = x;
                    (*_attributes[index].arc)[cp_index][1] = y;
                    (*_attributes[index].arc)[cp_index][2] = z;

                    (*_attributes[index].next->arc)[3] = (*_attributes[index].arc)[0];

                    (*_attributes[index].next->arc)[2] = 2 * (*_attributes[index].arc)[0] - (*_attributes[index].arc)[1];


                }

                if (next_connection == RIGHT)
                {
                    (*_attributes[index].arc)[cp_index][0] = x;
                    (*_attributes[index].arc)[cp_index][1] = y;
                    (*_attributes[index].arc)[cp_index][2] = z;

                    (*_attributes[index].next->arc)[0] = (*_attributes[index].arc)[0];

                    (*_attributes[index].next->arc)[1] = 2 * (*_attributes[index].arc)[0] - (*_attributes[index].arc)[1];

                }

                delete _attributes[index].image; _attributes[index].image = nullptr;
                _attributes[index].image = _attributes[index].arc->GenerateImage(_arc_max_order_of_derivatives, _arc_div_point_count, usage_flag);

                if (!_attributes[index].image)
                {
                    DeleteAllArcs();
                    throw Exception("Error occured during the image creation of the arc!");

                }

                if (!_attributes[index].image->UpdateVertexBufferObjects(_arc_scale))
                {
                    DeleteAllArcs();
                    throw Exception("Error occured during the update of the VBOs of the arc!");
                }

                if (!_attributes[index].arc->UpdateVertexBufferObjectsOfData())
                {
                    DeleteAllArcs();
                    throw Exception("Error occured during the update of the VBOs of the arc!");
                }

                delete _attributes[index].next->image; _attributes[index].next->image = nullptr;
                _attributes[index].next->image = _attributes[index].next->arc->GenerateImage(_arc_max_order_of_derivatives, _arc_div_point_count, usage_flag);

                if (!_attributes[index].next->image)
                {
                    DeleteAllArcs();
                    throw Exception("Error occured during the image creation of the arc!");

                }

                if (!_attributes[index].next->image->UpdateVertexBufferObjects(_arc_scale))
                {
                    DeleteAllArcs();
                    throw Exception("Error occured during the update of the VBOs of the arc!");
                }

                if (!_attributes[index].next->arc->UpdateVertexBufferObjectsOfData())
                {
                    DeleteAllArcs();
                    throw Exception("Error occured during the update of the VBOs of the arc!");
                }
            }

            if (cp_index == 2 && _attributes[index].previous)
            {
                Direction prev_connection = _attributes[index].previous_connection_type;

                if (prev_connection == LEFT)
                {
                    (*_attributes[index].arc)[cp_index][0] = x;
                    (*_attributes[index].arc)[cp_index][1] = y;
                    (*_attributes[index].arc)[cp_index][2] = z;

                    (*_attributes[index].previous->arc)[3] = (*_attributes[index].arc)[3];

                    (*_attributes[index].previous->arc)[2] = 2 * (*_attributes[index].arc)[3] - (*_attributes[index].arc)[2];


                }

                if (prev_connection == RIGHT)
                {
                    (*_attributes[index].arc)[cp_index][0] = x;
                    (*_attributes[index].arc)[cp_index][1] = y;
                    (*_attributes[index].arc)[cp_index][2] = z;

                    (*_attributes[index].previous->arc)[0] = (*_attributes[index].arc)[3];

                    (*_attributes[index].previous->arc)[1] = 2 * (*_attributes[index].arc)[3] - (*_attributes[index].arc)[2];

                }

                delete _attributes[index].image; _attributes[index].image = nullptr;
                _attributes[index].image = _attributes[index].arc->GenerateImage(_arc_max_order_of_derivatives, _arc_div_point_count, usage_flag);

                if (!_attributes[index].image)
                {
                    DeleteAllArcs();
                    throw Exception("Error occured during the image creation of the arc!");

                }

                if (!_attributes[index].image->UpdateVertexBufferObjects(_arc_scale))
                {
                    DeleteAllArcs();
                    throw Exception("Error occured during the update of the VBOs of the arc!");
                }

                if (!_attributes[index].arc->UpdateVertexBufferObjectsOfData())
                {
                    DeleteAllArcs();
                    throw Exception("Error occured during the update of the VBOs of the arc!");
                }

                delete _attributes[index].previous->image; _attributes[index].previous->image = nullptr;
                _attributes[index].previous->image = _attributes[index].previous->arc->GenerateImage(_arc_max_order_of_derivatives, _arc_div_point_count, usage_flag);

                if (!_attributes[index].previous->image)
                {
                    DeleteAllArcs();
                    throw Exception("Error occured during the image creation of the arc!");

                }

                if (!_attributes[index].previous->image->UpdateVertexBufferObjects(_arc_scale))
                {
                    DeleteAllArcs();
                    throw Exception("Error occured during the update of the VBOs of the arc!");
                }

                if (!_attributes[index].previous->arc->UpdateVertexBufferObjectsOfData())
                {
                    DeleteAllArcs();
                    throw Exception("Error occured during the update of the VBOs of the arc!");
                }
            }



        }
    }
    catch (Exception &e)
    {
        cout << e << endl;
    }
}

GLuint FirstOrderAlgebraicTrigonometricCompositeCurve3::GetDivPointCount() const
{
    return _arc_div_point_count;
}

GLboolean FirstOrderAlgebraicTrigonometricCompositeCurve3::SetDivPointCount(GLuint div_point_count)
{
    _arc_div_point_count = div_point_count;
    return GL_TRUE;
}

GLdouble FirstOrderAlgebraicTrigonometricCompositeCurve3::GetScale() const
{
    return _arc_scale;
}

GLboolean FirstOrderAlgebraicTrigonometricCompositeCurve3::SetScale(GLdouble value)
{
    _arc_scale = value;
    return GL_TRUE;
}


DCoordinate3 FirstOrderAlgebraicTrigonometricCompositeCurve3::GetSelectedArcSelectedControlPoint(GLuint index, GLuint cp_index) const
{
    return (*_attributes[index].arc)[cp_index];
}

GLboolean FirstOrderAlgebraicTrigonometricCompositeCurve3::SetAlpha(GLdouble alpha)
{
    _alpha = alpha;
    return GL_TRUE;
}

GLdouble FirstOrderAlgebraicTrigonometricCompositeCurve3::GetAlpha() const
{
    return _alpha;
}

GLuint FirstOrderAlgebraicTrigonometricCompositeCurve3::GetArcCount() const
{
    return _arc_count;
}

GLboolean FirstOrderAlgebraicTrigonometricCompositeCurve3::SetAlphaOfArcs()
{
    for (GLuint i = 0; i < _arc_count; ++i)
    {
       _attributes[i].arc->SetAlpha(_alpha);
    }
    return GL_TRUE;
}

void FirstOrderAlgebraicTrigonometricCompositeCurve3::SetAlphaAndRenderArcs(double alpha, GLenum usage_flag)
{
    try
    {
        _alpha = alpha;
        SetAlphaOfArcs();

        for (GLuint i = 0; i < _arc_count; ++i)
        {
             delete _attributes[i].image; _attributes[i].image = nullptr;
            _attributes[i].image = _attributes[i].arc->GenerateImage(_arc_max_order_of_derivatives, _arc_div_point_count, usage_flag);
            if(!_attributes[i].image)
            {
                DeleteAllArcs();
                throw Exception("Error occured during the image creation of the arc!");
            }

            if (!_attributes[i].image->UpdateVertexBufferObjects(_arc_scale))
            {
               DeleteAllArcs();
                throw Exception("Error occured during the update of the VBOs of the arc!");
            }
        }
    }  catch (Exception &e) {
        cout << e << endl;
    }
}

void FirstOrderAlgebraicTrigonometricCompositeCurve3::RenderArcsWithModifiedDivPointCount(GLenum usage_flag)
{
    try
    {
        for (GLuint i = 0; i < _arc_count; ++i)
        {
             delete _attributes[i].image; _attributes[i].image = nullptr;
            _attributes[i].image = _attributes[i].arc->GenerateImage(_arc_max_order_of_derivatives, _arc_div_point_count, usage_flag);
            if(!_attributes[i].image)
            {
                DeleteAllArcs();
                throw Exception("Error occured during the image creation of the arc!");
            }

            if (!_attributes[i].image->UpdateVertexBufferObjects(_arc_scale))
            {
               DeleteAllArcs();
                throw Exception("Error occured during the update of the VBOs of the arc!");
            }
        }
    }  catch (Exception &e) {
        cout << e << endl;
    }
}


GLboolean FirstOrderAlgebraicTrigonometricCompositeCurve3::JoinExistingArcs(GLuint index_0, Direction direction_0, GLuint index_1, Direction direction_1)
{
    assert((direction_0 == RIGHT || direction_0 == LEFT) && (direction_1 == RIGHT || direction_1 == LEFT) && index_0 >= 0 && index_0 <= _arc_count && index_1 >= 0 && index_1 <= _arc_count);

    if (direction_0 == LEFT && _attributes[index_0].previous != nullptr)
        return false;

    if (direction_0 == RIGHT && _attributes[index_0].next != nullptr)
        return false;

    if (direction_1 == LEFT && _attributes[index_1].previous != nullptr)
        return false;

    if (direction_1 == RIGHT && _attributes[index_1].next != nullptr)
        return false;

    _attributes[_arc_count].arc = new (nothrow) FirstOrderAlgebraicTrigonometricArc3();
    _attributes[_arc_count].color = new Color4(0.0, 1.0, 0.0);

    if(!_attributes[_arc_count].arc)
    {
        DeleteExistingArc(_arc_count);
        return false;
    }

    if (direction_0 == RIGHT)
    {
        (*_attributes[_arc_count].arc)[3] = (*_attributes[index_0].arc)[0];
        (*_attributes[_arc_count].arc)[2] = 2 * (*_attributes[index_0].arc)[0] - (*_attributes[index_0].arc)[1];

        if (direction_1 == RIGHT)
        {
            (*_attributes[_arc_count].arc)[0] = (*_attributes[index_1].arc)[0];
            (*_attributes[_arc_count].arc)[1] = 2 * (*_attributes[index_1].arc)[0] - (*_attributes[index_1].arc)[1];

            _attributes[_arc_count].previous = &_attributes[index_0];
            _attributes[_arc_count].next = &_attributes[index_1];
            _attributes[_arc_count].previous_connection_type = RIGHT;
            _attributes[_arc_count].next_connection_type = RIGHT;

            _attributes[index_0].next = &_attributes[_arc_count];
            _attributes[index_0].next_connection_type = LEFT;

            _attributes[index_1].next = &_attributes[_arc_count];
            _attributes[index_1].next_connection_type = RIGHT;
        }
        else
        {
            (*_attributes[_arc_count].arc)[0] = (*_attributes[index_1].arc)[3];
            (*_attributes[_arc_count].arc)[1] = 2 * (*_attributes[index_1].arc)[3] - (*_attributes[index_1].arc)[2];

            _attributes[_arc_count].previous = &_attributes[index_0];
            _attributes[_arc_count].next = &_attributes[index_1];
            _attributes[_arc_count].previous_connection_type = RIGHT;
            _attributes[_arc_count].next_connection_type = LEFT;

            _attributes[index_0].next = &_attributes[_arc_count];
            _attributes[index_0].next_connection_type = LEFT;

            _attributes[index_1].previous = &_attributes[_arc_count];
            _attributes[index_1].previous_connection_type = RIGHT;
        }
    }

    else
    {
        (*_attributes[_arc_count].arc)[0] = (*_attributes[index_0].arc)[3];
        (*_attributes[_arc_count].arc)[1] = 2 * (*_attributes[index_0].arc)[3] - (*_attributes[index_0].arc)[2];

        if (direction_1 == RIGHT)
        {
            (*_attributes[_arc_count].arc)[3] = (*_attributes[index_1].arc)[0];
            (*_attributes[_arc_count].arc)[2] = 2 * (*_attributes[index_1].arc)[0] - (*_attributes[index_1].arc)[1];

            _attributes[_arc_count].previous = &_attributes[index_1];
            _attributes[_arc_count].next = &_attributes[index_0];
            _attributes[_arc_count].previous_connection_type = RIGHT;
            _attributes[_arc_count].next_connection_type = LEFT;

            _attributes[index_0].previous = &_attributes[_arc_count];
            _attributes[index_0].previous_connection_type = RIGHT;

            _attributes[index_1].next = &_attributes[_arc_count];
            _attributes[index_1].next_connection_type = LEFT;
        }
        else
        {
            (*_attributes[_arc_count].arc)[3] = (*_attributes[index_1].arc)[3];
            (*_attributes[_arc_count].arc)[2] = 2 * (*_attributes[index_1].arc)[3] - (*_attributes[index_1].arc)[2];

            _attributes[_arc_count].previous = &_attributes[index_1];
            _attributes[_arc_count].next = &_attributes[index_0];
            _attributes[_arc_count].previous_connection_type = LEFT;
            _attributes[_arc_count].next_connection_type = LEFT;

            _attributes[index_0].previous = &_attributes[_arc_count];
            _attributes[index_0].previous_connection_type = RIGHT;

            _attributes[index_1].previous = &_attributes[_arc_count];
            _attributes[index_1].previous_connection_type = LEFT;
        }
    }


    _attributes[_arc_count].arc->UpdateVertexBufferObjectsOfData();

    _attributes[_arc_count].image = _attributes[_arc_count].arc->GenerateImage(2, _arc_div_point_count);

    if (!_attributes[_arc_count].image)
    {
         DeleteExistingArc(_arc_count);
         return false;
    }

    _attributes[_arc_count].image->UpdateVertexBufferObjects();

    _arc_count++;
    return true;

}

GLboolean FirstOrderAlgebraicTrigonometricCompositeCurve3::InsertLine(Color4 *color, GLuint max_order_of_derivatives, GLuint div_point_count, GLenum usage_flag)
{
    DCoordinate3 *pontok = new DCoordinate3[4];
    pontok[0].x() = -2.0;
    pontok[1].x() = -0.66;
    pontok[2].x() = 0.66;
    pontok[3].x() = 2.0;
    InsertNewArc2(pontok, color, max_order_of_derivatives, div_point_count, usage_flag);
    return true;
}

GLboolean FirstOrderAlgebraicTrigonometricCompositeCurve3::InsertNewArc2(
        DCoordinate3 *pontok,
        Color4 *color, GLuint max_order_of_derivatives, GLuint div_point_count, GLenum usage_flag)
{
    GLuint index = _arc_count;
    _attributes[index].arc = new (nothrow) FirstOrderAlgebraicTrigonometricArc3();
    _attributes[index].color = color;

    if(!_attributes[index].arc)
    {
        DeleteExistingArc(index);
        return false;
    }

    for (GLuint i = 0; i < 4; i++)
    {
        (*_attributes[index].arc)[i] = pontok[i];
    }

    _attributes[index].arc->UpdateVertexBufferObjectsOfData();


    _attributes[index].image = _attributes[index].arc->GenerateImage(max_order_of_derivatives, div_point_count, usage_flag);

    if (!_attributes[index].image)
    {
         DeleteExistingArc(index);
         return false;
    }

    _attributes[index].image->UpdateVertexBufferObjects();

    _arc_count ++;

    return true;
}

GLboolean FirstOrderAlgebraicTrigonometricCompositeCurve3::ContinueExisitingArc(GLuint index, Direction direction)
{
    DCoordinate3 *coord = new DCoordinate3[4];

    if (direction == LEFT)
    {
        for (GLuint i = 0; i < 4; i ++)
        {
            coord[i] = (*_attributes[index].arc)[3] + i * ((*_attributes[index].arc)[3] - (*_attributes[index].arc)[2]);
        }

    }
    else
    {
        for (GLuint i = 0; i < 4; i ++)
        {
            coord[i] = (*_attributes[index].arc)[0] + i * ((*_attributes[index].arc)[0] - (*_attributes[index].arc)[1]);
        }
    }

    constexpr int FLOAT_MIN = 0;
    constexpr int FLOAT_MAX = 1;

    random_device rd;
    default_random_engine eng(rd());
    uniform_real_distribution<float> distr(FLOAT_MIN, FLOAT_MAX);

    Color4 *c = new Color4(distr(eng), distr(eng), distr(eng));
    InsertNewArc2(coord, c, 2, 100);

    if (direction == LEFT)
    {
        _attributes[index].previous =  &_attributes[_arc_count-1];
        _attributes[index].previous_connection_type = RIGHT;
        _attributes[_arc_count-1].next = &_attributes[index];
        _attributes[_arc_count-1].next_connection_type = LEFT;

    }
    else
    {
        _attributes[index].next =  &_attributes[_arc_count-1];
        _attributes[index].next_connection_type = RIGHT;
        _attributes[_arc_count-1].next = &_attributes[index];
        _attributes[_arc_count-1].next_connection_type = RIGHT;
    }
    return true;
}


void FirstOrderAlgebraicTrigonometricCompositeCurve3::set_selected_arc_index(GLuint index)
{
    _selected_arc_index = index;
}

void FirstOrderAlgebraicTrigonometricCompositeCurve3::set_selected_cp(GLuint index)
{
    _selected_cp = index;
}
