#include "FirstOrderAlgebraicTrigonometricCompositePatch3.h"
#include <iostream>
#include <fstream>

using namespace cagd;
using namespace std;

FirstOrderAlgebraicTrigonometricCompositePatch3::FirstOrderAlgebraicTrigonometricCompositePatch3(GLdouble u_alpha, GLdouble v_alpha, GLuint minimial_patch_count_to_be_reserved)
{
    _alpha[0] = u_alpha;
    _alpha[1] = v_alpha;
//    _attributes.reserve(minimial_patch_count_to_be_reserved);
    _attributes.resize(minimial_patch_count_to_be_reserved);
}

GLboolean FirstOrderAlgebraicTrigonometricCompositePatch3::InsertNewPatch(Material *material, GLenum usage_flag)
{
    cout << _patch_count << endl;
    GLuint index = _patch_count;
    _attributes[index].patch = new (nothrow) FirstOrderAlgebraicTrigonometricPatch3();
    _attributes[index].material = material;

    if(!_attributes[index].patch)
    {
        DeleteExistingPatch(index);
        return GL_FALSE;
    }

    _attributes[index].patch->SetData(0, 0, -2.0, -2.0, 0.0);
    _attributes[index].patch->SetData(0, 1, -2.0, -1.0, 0.0);
    _attributes[index].patch->SetData(0, 2, -2.0, 1.0, 0.0);
    _attributes[index].patch->SetData(0, 3, -2.0, 2.0, 0.0);

    _attributes[index].patch->SetData(1, 0, -1.0, -2.0, 0.0);
    _attributes[index].patch->SetData(1, 1, -1.0, -1.0, 2.0);
    _attributes[index].patch->SetData(1, 2, -1.0, 1.0, 2.0);
    _attributes[index].patch->SetData(1, 3, -1.0, 2.0, 0.0);

    _attributes[index].patch->SetData(2, 0, 1.0, -2.0, 0.0);
    _attributes[index].patch->SetData(2, 1, 1.0, -1.0, 2.0);
    _attributes[index].patch->SetData(2, 2, 1.0, 1.0, 2.0);
    _attributes[index].patch->SetData(2, 3, 1.0, 2.0, 0.0);

    _attributes[index].patch->SetData(3, 0, 2.0, -2.0, 0.0);
    _attributes[index].patch->SetData(3, 1, 2.0, -1.0, 0.0);
    _attributes[index].patch->SetData(3, 2, 2.0, 1.0, 0.0);
    _attributes[index].patch->SetData(3, 3, 2.0, 2.0, 0.0);



    _attributes[index].patch->UpdateVertexBufferObjectsOfData();
    _attributes[index].image = _attributes[index].patch->GenerateImage(30, 30, usage_flag);

    if (_attributes[index].image)
        _attributes[index].image->UpdateVertexBufferObjects();

    _attributes[index]._uIsoparametricLines = _attributes[index].patch->GenerateUIsoparametricLines(_uIsoLineCount, 2, _uDivPointCount);
    _attributes[index]._vIsoparametricLines = _attributes[index].patch->GenerateVIsoparametricLines(_vIsoLineCount, 2, _vDivPointCount);

    for (GLuint i = 0; i < _attributes[index]._uIsoparametricLines->GetColumnCount(); i++)
    {
        (*_attributes[index]._uIsoparametricLines)[i]->UpdateVertexBufferObjects();
    }

    for (GLuint i = 0; i < _attributes[index]._vIsoparametricLines->GetColumnCount(); i++)
    {
        (*_attributes[index]._vIsoparametricLines)[i]->UpdateVertexBufferObjects();
    }

    _patch_count++;

    return GL_TRUE;

}

GLboolean FirstOrderAlgebraicTrigonometricCompositePatch3::DeleteExistingPatch(GLuint index)
{
    if (PatchExists(index))
    {
        for (GLuint i = 0; i < 8; ++i)
        {
           if (_attributes[index].neighbours[i] != nullptr)
           {
               for (GLuint j = 0; j < 8; ++j)
               {
                   cout << _attributes[index].connection_type[j] << endl;
                   if (_attributes[index].connection_type[j] >= 0 && _attributes[index].connection_type[j] <= 7)
                   {
                       _attributes[index].neighbours[i]->neighbours[_attributes[index].connection_type[j]] = nullptr;
                   }
               }
           }
        }

        if (_attributes[index].patch)
        {
            delete _attributes[index].patch; _attributes[index].patch = nullptr;
        }

        if (_attributes[index].image)
        {
            delete _attributes[index].image; _attributes[index].image = nullptr;
        }

        if (_attributes[index]._uIsoparametricLines)
        {
            for (GLuint j = 0; j < _attributes[index]._uIsoparametricLines->GetColumnCount(); ++j){
                delete (* _attributes[index]._uIsoparametricLines)[j];
                (* _attributes[index]._uIsoparametricLines)[j] = nullptr;
            }
            delete _attributes[index]._uIsoparametricLines;
            _attributes[index]._uIsoparametricLines = nullptr;
        }

        if (_attributes[index]._vIsoparametricLines)
        {
            for (GLuint j = 0; j < _attributes[index]._vIsoparametricLines->GetColumnCount(); ++j){
                delete (* _attributes[index]._vIsoparametricLines)[j];
                (* _attributes[index]._vIsoparametricLines)[j] = nullptr;
            }
            delete _attributes[index]._vIsoparametricLines;
            _attributes[index]._vIsoparametricLines = nullptr;
        }

        if (_attributes[index].texture)
        {
            delete _attributes[index].texture; _attributes[index].texture = nullptr;
        }

        /*if (_attributes[index].material)
        {
            delete _attributes[index].material; _attributes[index].material = nullptr;
        }*/

        /*for (GLuint i = 0; i < 8; ++i)
        {
            if (_attributes[index].neighbours[i])
            {
                delete _attributes[index].neighbours[i]; _attributes[index].neighbours[i] = nullptr;
            }
        }*/
    }

    return GL_TRUE;
}

GLboolean FirstOrderAlgebraicTrigonometricCompositePatch3::PatchExists(GLuint index) const
{
    if (_attributes[index].patch)
    {
        return GL_TRUE;
    }

    return GL_FALSE;
}

GLboolean FirstOrderAlgebraicTrigonometricCompositePatch3::RenderAllPatches(GLuint order, GLenum render_mode) const
{
    for (GLuint i = 0; i < _patch_count; i++)
    {
        if (PatchExists(i))
        {
            RenderSelectedPatch(i, order, render_mode);
            //RenderColors(i);
        }
    }

    return GL_TRUE;
}


GLboolean FirstOrderAlgebraicTrigonometricCompositePatch3::RenderSelectedPatch(GLuint index, GLuint order, GLenum render_mode) const
{
    assert(index >= 0 && index <= _attributes.size());

    if (_show_texture && _attributes[index].texture)
    {
        glEnable(GL_TEXTURE_2D);
        _attributes[index].texture->bind();

    }
    MatFBTurquoise.Apply();
    _attributes[index].patch->RenderData(GL_LINE_STRIP);

    if (_attributes[index].image)
    {
        if (index != _selected_patch_index)
        {
            _attributes[index].material->Apply();
        }
        else
        {
            MatFBGold.Apply();
        }
        _attributes[index].image->Render();
        if (_visibility_of_normal_vectors)
        {
            MatFBGold.Apply();
            _attributes[index].image->RenderNormals(_normalScale);
        }
    }

    glDisable(GL_TEXTURE_2D);

    MatFBTurquoise.Apply();
    for (GLuint i = 0; i < _attributes[index]._uIsoparametricLines->GetColumnCount(); i++)
    {
        glColor3f(1.0f, 0.0f, 0.0f);
        (*_attributes[index]._uIsoparametricLines)[i]->RenderDerivatives(order, render_mode);
    }

    for (GLuint i = 0; i < _attributes[index]._vIsoparametricLines->GetColumnCount(); i++)
    {
        glColor3f(1.0f, 0.0f, 0.0f);
        (*_attributes[index]._vIsoparametricLines)[i]->RenderDerivatives(order, render_mode);
    }

    return GL_TRUE;
}

GLboolean FirstOrderAlgebraicTrigonometricCompositePatch3::RenderColors() const
{
    for (GLuint i = 0; i < _patch_count; i ++)
    {
        if (PatchExists(i))
        {
            glDisable(GL_LIGHTING);
            glDisable(GL_LIGHT0);
            glDisable(GL_NORMALIZE);
            glPointSize(40.0f);
            glBegin(GL_POINTS);
                double x, y, z;
                _attributes[i].patch->GetData(0, 0, x, y, z);       //NW - pink
                glColor3f(1.00f, 0.43f, 0.78f);
                glVertex3f(x, y, z);

                _attributes[i].patch->GetData(0, 1, x, y, z);       //N - piros
                glColor3f(1.0f, 0.0f, 0.0f);
                glVertex3f(x, y, z);

                _attributes[i].patch->GetData(0, 2, x, y, z);       //N - piros
                glColor3f(1.0f, 0.0f, 0.0f);
                glVertex3f(x, y, z);

                _attributes[i].patch->GetData(0, 3, x, y, z);       //NE -barna
                glColor3f(0.36f, 0.25f, 0.20f);
                glVertex3f(x, y, z);

                _attributes[i].patch->GetData(1, 0, x, y, z);       //W - narancs
                glColor3f(1.0f, 0.5f, 0.0f);
                glVertex3f(x, y, z);

                _attributes[i].patch->GetData(2, 0, x, y, z);       //W - narancs
                glColor3f(1.0f, 0.5f, 0.0f);
                glVertex3f(x, y, z);

                _attributes[i].patch->GetData(1, 3, x, y, z);       //E - zold
                glColor3f(0.0f, 1.0f, 0.0f);
                glVertex3f(x, y, z);

                _attributes[i].patch->GetData(2, 3, x, y, z);       //E - zold
                glColor3f(0.0f, 1.0f, 0.0f);
                glVertex3f(x, y, z);

                _attributes[i].patch->GetData(3, 3, x, y, z);       //SE - kek
                glColor3f(0.0f, 0.0f, 1.0f);
                glVertex3f(x, y, z);

                _attributes[i].patch->GetData(3, 2, x, y, z);       //S - lila
                glColor3f(0.53f, 0.12f, 0.47f);
                glVertex3f(x, y, z);

                _attributes[i].patch->GetData(3, 1, x, y, z);       //S - lila
                glColor3f(0.53f, 0.12f, 0.47f);
                glVertex3f(x, y, z);

                _attributes[i].patch->GetData(3, 0, x, y, z);       //SW - sarga
                glColor3f(1.0f, 1.0f, 0.0f);
                glVertex3f(x, y, z);
            glEnd();
            glEnable(GL_LIGHTING);
            glEnable(GL_LIGHT0);
            glEnable(GL_NORMALIZE);
        }
    }

    return GL_TRUE;
}


void FirstOrderAlgebraicTrigonometricCompositePatch3::DeleteAllPatches()
{
    for (GLuint i = 0; i < _patch_count; i++)
    {
        DeleteExistingPatch(i);
    }
}

void FirstOrderAlgebraicTrigonometricCompositePatch3::set_u_div_point_count(GLuint value)
{
    if (_uDivPointCount != value)
    {
        _uDivPointCount = value;
        for (GLuint i = 0; i < _patch_count; i++)
        {
            if (PatchExists(i))
            {
                ModifyUV(0, i);
            }
        }
    }
}

void FirstOrderAlgebraicTrigonometricCompositePatch3::set_u_iso_line_count(GLuint value)
{
    if (_uIsoLineCount != value)
    {
        _uIsoLineCount = value;
        for (GLuint i = 0; i < _patch_count; i++)
        {
            if (PatchExists(i))
            {
                ModifyUV(0, i);
            }
        }
    }
}

void FirstOrderAlgebraicTrigonometricCompositePatch3::set_v_div_point_count(GLuint value)
{
    if (_vDivPointCount != value)
    {
        _vDivPointCount = value;
        for (GLuint i = 0; i < _patch_count; i++)
        {
            if (PatchExists(i))
            {
                ModifyUV(1, i);
            }
        }
    }
}

void FirstOrderAlgebraicTrigonometricCompositePatch3::set_v_iso_line_count(GLuint value)
{
    if (_vIsoLineCount != value)
    {
        _vIsoLineCount = value;
        for (GLuint i = 0; i < _patch_count; i++)
        {
            if (PatchExists(i))
            {
                ModifyUV(1, i);
            }
        }
    }
}

void FirstOrderAlgebraicTrigonometricCompositePatch3::set_uv_scale(GLdouble value)
{
    if (_uvScale != value)
    {
        _uvScale = value;
        for (GLuint i = 0; i < _patch_count; i++)
        {
            if (PatchExists(i))
            {
                ModifyUV(2, i);
            }
        }
    }
}

void FirstOrderAlgebraicTrigonometricCompositePatch3::set_normal_scale(GLdouble value)
{
    if (_normalScale != value)
    {
        _normalScale = value;
        for (GLuint i = 0; i < _patch_count; i++)
        {
            if (PatchExists(i))
            {
                ModifyUV(2, i);
            }
        }
    }
}

void FirstOrderAlgebraicTrigonometricCompositePatch3::set_visibility_of_normal_vectors(bool value)
{
    _visibility_of_normal_vectors = value;
}

void FirstOrderAlgebraicTrigonometricCompositePatch3::ModifyUV(GLuint value, GLuint index)
{
    if (value == 0 || value == 2)
    {
        _attributes[index]._uIsoparametricLines = _attributes[index].patch->GenerateUIsoparametricLines(_uIsoLineCount, 2, _uDivPointCount);
        for (GLuint i = 0; i < _attributes[index]._uIsoparametricLines->GetColumnCount(); i++)
        {
            (*_attributes[index]._uIsoparametricLines)[i]->UpdateVertexBufferObjects(_uvScale);
        }
    }

    if (value == 1 || value == 2)
    {
        _attributes[index]._vIsoparametricLines = _attributes[index].patch->GenerateVIsoparametricLines(_vIsoLineCount, 2, _vDivPointCount);
        for (GLuint i = 0; i < _attributes[index]._vIsoparametricLines->GetColumnCount(); i++)
        {
            (*_attributes[index]._vIsoparametricLines)[i]->UpdateVertexBufferObjects(_uvScale);
        }
    }
}

void FirstOrderAlgebraicTrigonometricCompositePatch3::MoveControlPoint(int index, int r, int c, bool &new_image_1, Direction &direction_1)
{
    new_image_1 = false;

    DCoordinate3 cp;
    _attributes[index].patch->GetData(r, c, cp);

    if (r == 0 && c == 1)
    {
        if (_attributes[index].neighbours[N] != nullptr)
        {
            new_image_1 = true;
            direction_1 = N;

            if(_attributes[index].connection_type[N] == N)
            {
                 _attributes[index].neighbours[N]->patch->SetData(0, 1, cp);

                 DCoordinate3 cp11 = (*_attributes[index].patch)(1, 1);

                 _attributes[index].neighbours[N]->patch->SetData(1, 1, 2 * cp - cp11);
            }

            if(_attributes[index].connection_type[N] == E)
            {
                 _attributes[index].neighbours[N]->patch->SetData(2, 3, cp);

                 DCoordinate3 cp11 = (*_attributes[index].patch)(1, 1);

                 _attributes[index].neighbours[N]->patch->SetData(2, 2, 2 * cp - cp11);
            }

            if(_attributes[index].connection_type[N] == S)
            {
                 _attributes[index].neighbours[N]->patch->SetData(3, 1, cp);

                 DCoordinate3 cp11 = (*_attributes[index].patch)(1, 1);

                 _attributes[index].neighbours[N]->patch->SetData(2, 1, 2 * cp - cp11);
            }

            if(_attributes[index].connection_type[N] == W)
            {
                 _attributes[index].neighbours[N]->patch->SetData(2, 0, cp);

                 DCoordinate3 cp11 = (*_attributes[index].patch)(1, 1);

                 _attributes[index].neighbours[N]->patch->SetData(2, 1, 2 * cp - cp11);
            }
        }

    }

    if (r == 0 && c == 2)
    {
        if (_attributes[index].neighbours[N] != nullptr)
        {
            new_image_1 = true;
            direction_1 = N;

            if(_attributes[index].connection_type[N] == N)
            {
                 _attributes[index].neighbours[N]->patch->SetData(0, 2, cp);

                 DCoordinate3 cp12 = (*_attributes[index].patch)(1, 2);

                 _attributes[index].neighbours[N]->patch->SetData(1, 2, 2 * cp - cp12);
            }

            if(_attributes[index].connection_type[N] == E)
            {
                 _attributes[index].neighbours[N]->patch->SetData(1, 3, cp);

                 DCoordinate3 cp12 = (*_attributes[index].patch)(1, 2);

                 _attributes[index].neighbours[N]->patch->SetData(1, 2, 2 * cp - cp12);
            }

            if(_attributes[index].connection_type[N] == S)
            {
                 _attributes[index].neighbours[N]->patch->SetData(3, 2, cp);

                 DCoordinate3 cp12 = (*_attributes[index].patch)(1, 2);

                 _attributes[index].neighbours[N]->patch->SetData(2, 2, 2 * cp - cp12);
            }

            if(_attributes[index].connection_type[N] == W)
            {
                 _attributes[index].neighbours[N]->patch->SetData(2, 0, cp);

                 DCoordinate3 cp12 = (*_attributes[index].patch)(1, 2);

                 _attributes[index].neighbours[N]->patch->SetData(2, 1, 2 * cp - cp12);
            }
        }

    }

    if (r == 1 && c == 0)
    {
        if (_attributes[index].neighbours[W] != nullptr)
        {
            new_image_1 = true;
            direction_1 = W;

            if(_attributes[index].connection_type[W] == N)
            {
                 _attributes[index].neighbours[W]->patch->SetData(0, 1, cp);

                 DCoordinate3 cp11 = (*_attributes[index].patch)(1, 1);

                 _attributes[index].neighbours[W]->patch->SetData(1, 1, 2 * cp - cp11);
            }

            if(_attributes[index].connection_type[W] == E)
            {
                 _attributes[index].neighbours[W]->patch->SetData(1, 3, cp);

                 DCoordinate3 cp11 = (*_attributes[index].patch)(1, 1);

                 _attributes[index].neighbours[W]->patch->SetData(1, 2, 2 * cp - cp11);
            }

            if(_attributes[index].connection_type[W] == S)
            {
                 _attributes[index].neighbours[W]->patch->SetData(3, 2, cp);

                 DCoordinate3 cp11 = (*_attributes[index].patch)(1, 1);

                 _attributes[index].neighbours[W]->patch->SetData(2, 2, 2 * cp - cp11);
            }

            if(_attributes[index].connection_type[W] == W)
            {
                 _attributes[index].neighbours[W]->patch->SetData(1, 0, cp);

                 DCoordinate3 cp11 = (*_attributes[index].patch)(1, 1);

                 _attributes[index].neighbours[W]->patch->SetData(1, 1, 2 * cp - cp11);
            }
        }

    }

    if (r == 1 && c == 3)
    {
        if (_attributes[index].neighbours[E] != nullptr)
        {
            new_image_1 = true;
            direction_1 = E;

            if (_attributes[index].connection_type[E] == N)
            {
                _attributes[index].neighbours[E]->patch->SetData(0, 2, cp);

                DCoordinate3 cp12 = (*_attributes[index].patch)(1, 2);

                _attributes[index].neighbours[E]->patch->SetData(1, 2, 2 * cp - cp12);
            }

            if (_attributes[index].connection_type[E] == E)
            {
                _attributes[index].neighbours[E]->patch->SetData(1, 3, cp);

                DCoordinate3 cp12 = (*_attributes[index].patch)(1, 2);

                _attributes[index].neighbours[E]->patch->SetData(1, 2, 2 * cp - cp12);
            }

            if (_attributes[index].connection_type[E] == W)
            {
                _attributes[index].neighbours[E]->patch->SetData(1, 0, cp);

                DCoordinate3 cp12 = (*_attributes[index].patch)(1, 2);

                _attributes[index].neighbours[E]->patch->SetData(1, 1, 2 * cp - cp12);
            }

            if (_attributes[index].connection_type[E] == S)
            {
                _attributes[index].neighbours[E]->patch->SetData(3, 1, cp);

                DCoordinate3 cp12 = (*_attributes[index].patch)(1, 2);

                _attributes[index].neighbours[E]->patch->SetData(2, 1, 2 * cp - cp12);
            }
        }
    }


    if (r == 2 && c == 0)
    {
        if (_attributes[index].neighbours[W] != nullptr)
        {
            new_image_1 = true;
            direction_1 = W;

            if(_attributes[index].connection_type[W] == N)
            {
                 _attributes[index].neighbours[W]->patch->SetData(0, 2, cp);

                 DCoordinate3 cp21 = (*_attributes[index].patch)(2, 1);

                 _attributes[index].neighbours[W]->patch->SetData(1, 2, 2 * cp - cp21);
            }

            if(_attributes[index].connection_type[W] == E)
            {
                 _attributes[index].neighbours[W]->patch->SetData(2, 3, cp);

                 DCoordinate3 cp21 = (*_attributes[index].patch)(2, 1);

                 _attributes[index].neighbours[W]->patch->SetData(2, 2, 2 * cp - cp21);
            }

            if(_attributes[index].connection_type[W] == S)
            {
                 _attributes[index].neighbours[W]->patch->SetData(3, 1, cp);

                 DCoordinate3 cp21 = (*_attributes[index].patch)(2, 1);

                 _attributes[index].neighbours[W]->patch->SetData(2, 1, 2 * cp - cp21);
            }

            if(_attributes[index].connection_type[W] == W)
            {
                 _attributes[index].neighbours[W]->patch->SetData(2, 0, cp);

                 DCoordinate3 cp21 = (*_attributes[index].patch)(2, 1);

                 _attributes[index].neighbours[W]->patch->SetData(2, 1, 2 * cp - cp21);
            }
        }

    }


    if (r == 2 && c == 3)
    {
        if (_attributes[index].neighbours[E] != nullptr)
        {
            new_image_1 = true;
            direction_1 = E;

            if (_attributes[index].connection_type[E] == N)
            {
                _attributes[index].neighbours[E]->patch->SetData(0, 1, cp);

                DCoordinate3 cp22 = (*_attributes[index].patch)(2, 2);

                _attributes[index].neighbours[E]->patch->SetData(1, 1, 2 * cp - cp22);
            }

            if (_attributes[index].connection_type[E] == E)
            {
                _attributes[index].neighbours[E]->patch->SetData(2, 3, cp);

                DCoordinate3 cp22 = (*_attributes[index].patch)(2, 2);

                _attributes[index].neighbours[E]->patch->SetData(2, 2, 2 * cp - cp22);
            }

            if (_attributes[index].connection_type[E] == W)
            {
                _attributes[index].neighbours[E]->patch->SetData(2, 0, cp);

                DCoordinate3 cp22 = (*_attributes[index].patch)(2, 2);

                _attributes[index].neighbours[E]->patch->SetData(2, 1, 2 * cp - cp22);
            }

            if (_attributes[index].connection_type[E] == S)
            {
                _attributes[index].neighbours[E]->patch->SetData(3, 2, cp);

                DCoordinate3 cp22 = (*_attributes[index].patch)(2, 2);

                _attributes[index].neighbours[E]->patch->SetData(2, 2, 2 * cp - cp22);
            }
        }
    }


    if (r == 3 && c == 1)
    {
        if (_attributes[index].neighbours[S] != nullptr)
        {
            new_image_1 = true;
            direction_1 = S;

            if (_attributes[index].connection_type[S] == N)
            {
                _attributes[index].neighbours[S]->patch->SetData(0, 1, cp);

                DCoordinate3 cp21 = (*_attributes[index].patch)(2, 1);

                _attributes[index].neighbours[S]->patch->SetData(1, 1, 2 * cp - cp21);
            }

            if (_attributes[index].connection_type[S] == E)
            {
                _attributes[index].neighbours[S]->patch->SetData(1, 3, cp);

                DCoordinate3 cp21 = (*_attributes[index].patch)(2, 1);

                _attributes[index].neighbours[S]->patch->SetData(1, 2, 2 * cp - cp21);
            }

            if (_attributes[index].connection_type[S] == W)
            {
                _attributes[index].neighbours[S]->patch->SetData(2, 0, cp);

                DCoordinate3 cp21 = (*_attributes[index].patch)(2, 1);

                _attributes[index].neighbours[S]->patch->SetData(2, 1, 2 * cp - cp21);
            }

            if (_attributes[index].connection_type[S] == S)
            {
                _attributes[index].neighbours[S]->patch->SetData(3, 1, cp);

                DCoordinate3 cp21 = (*_attributes[index].patch)(2, 1);

                _attributes[index].neighbours[S]->patch->SetData(2, 1, 2 * cp - cp21);
            }
        }
    }

    if (r == 3 && c == 2)
    {
        if (_attributes[index].neighbours[S] != nullptr)
        {
            new_image_1 = true;
            direction_1 = S;

            if (_attributes[index].connection_type[S] == N)
            {
                _attributes[index].neighbours[S]->patch->SetData(0, 2, cp);

                DCoordinate3 cp22 = (*_attributes[index].patch)(2, 2);

                _attributes[index].neighbours[S]->patch->SetData(1, 2, 2 * cp - cp22);
            }

            if (_attributes[index].connection_type[S] == E)
            {
                _attributes[index].neighbours[S]->patch->SetData(2, 3, cp);

                DCoordinate3 cp22 = (*_attributes[index].patch)(2, 2);

                _attributes[index].neighbours[S]->patch->SetData(2, 2, 2 * cp - cp22);
            }

            if (_attributes[index].connection_type[S] == W)
            {
                _attributes[index].neighbours[S]->patch->SetData(1, 0, cp);

                DCoordinate3 cp22 = (*_attributes[index].patch)(2, 2);

                _attributes[index].neighbours[S]->patch->SetData(1, 1, 2 * cp - cp22);
            }

            if (_attributes[index].connection_type[S] == S)
            {
                _attributes[index].neighbours[S]->patch->SetData(3, 2, cp);

                DCoordinate3 cp22 = (*_attributes[index].patch)(2, 2);

                _attributes[index].neighbours[S]->patch->SetData(2, 2, 2 * cp - cp22);
            }
        }
    }
}

void FirstOrderAlgebraicTrigonometricCompositePatch3::Modify(int coord, int index, double value, int selected_row_index, int selected_column_index)
{
    if (_attributes[index].patch == nullptr)
    {
        return;
    }

    DCoordinate3 cp;
    _attributes[index].patch->GetData(selected_row_index, selected_column_index, cp);
    switch (coord)
    {
        case 0:
             cp[0] = value;
             break;
        case 1:
             cp[1] = value;
             break;
        case 2:
             cp[2] = value;
             break;
        default:
             break;
    }


//    {N, NW, W, SW, S, SE, E, NE};
    int r = selected_row_index;
    int c = selected_column_index;

    if ((r == 0 && c == 0 && _attributes[index].neighbours[NW] == nullptr && _attributes[index].neighbours[N] == nullptr && _attributes[index].neighbours[W] == nullptr) ||
            (r == 0 && c == 3 && _attributes[index].neighbours[NE] == nullptr && _attributes[index].neighbours[N] == nullptr && _attributes[index].neighbours[E] == nullptr) ||
            (r == 3 && c == 0 && _attributes[index].neighbours[SW] == nullptr && _attributes[index].neighbours[S] == nullptr && _attributes[index].neighbours[W] == nullptr) ||
            (r == 3 && c == 3 && _attributes[index].neighbours[SE] == nullptr && _attributes[index].neighbours[S] == nullptr && _attributes[index].neighbours[E] == nullptr) ||
            (r == 0 && c == 1 && _attributes[index].neighbours[N] == nullptr && _attributes[index].neighbours[NW] == nullptr) ||
            (r == 0 && c == 2 && _attributes[index].neighbours[N] == nullptr && _attributes[index].neighbours[NE] == nullptr) ||
            (r == 1 && c == 0 && _attributes[index].neighbours[W] == nullptr && _attributes[index].neighbours[NW] == nullptr) ||
            (r == 2 && c == 0 && _attributes[index].neighbours[W] == nullptr && _attributes[index].neighbours[SW] == nullptr) ||
            (r == 3 && c == 1 && _attributes[index].neighbours[S] == nullptr && _attributes[index].neighbours[SW] == nullptr) ||
            (r == 3 && c == 2 && _attributes[index].neighbours[S] == nullptr && _attributes[index].neighbours[SE] == nullptr) ||
            (r == 2 && c == 3 && _attributes[index].neighbours[E] == nullptr && _attributes[index].neighbours[SE] == nullptr) ||
            (r == 1 && c == 3 && _attributes[index].neighbours[E] == nullptr && _attributes[index].neighbours[NE] == nullptr) ||
            (r == 1 && c == 1 && _attributes[index].neighbours[N] == nullptr && _attributes[index].neighbours[W] == nullptr) ||
            (r == 2 && c == 1 && _attributes[index].neighbours[W] == nullptr && _attributes[index].neighbours[S] == nullptr) ||
            (r == 1 && c == 2 && _attributes[index].neighbours[N] == nullptr && _attributes[index].neighbours[E] == nullptr) ||
            (r == 2 && c == 2 && _attributes[index].neighbours[E] == nullptr && _attributes[index].neighbours[S] == nullptr))
    {
        if (_attributes[index].image)
        {
            delete _attributes[index].image; _attributes[index].image = nullptr;
        }

        _attributes[index].patch->SetData(selected_row_index, selected_column_index, cp);

        _attributes[index].image = _attributes[index].patch->GenerateImage(_uDivPointCount, _vDivPointCount, GL_STATIC_DRAW);

        if (_attributes[index].image)
            _attributes[index].image->UpdateVertexBufferObjects();

        _attributes[index]._uIsoparametricLines = _attributes[index].patch->GenerateUIsoparametricLines(_uIsoLineCount, 2, _uDivPointCount);
        _attributes[index]._vIsoparametricLines = _attributes[index].patch->GenerateVIsoparametricLines(_vIsoLineCount, 2, _vDivPointCount);

        for (GLuint i = 0; i < _attributes[index]._uIsoparametricLines->GetColumnCount(); i++)
        {
            (*_attributes[index]._uIsoparametricLines)[i]->UpdateVertexBufferObjects();
        }

        for (GLuint i = 0; i < _attributes[index]._vIsoparametricLines->GetColumnCount(); i++)
        {
            (*_attributes[index]._vIsoparametricLines)[i]->UpdateVertexBufferObjects();
        }

        //_patch.GetData(_selected_row_index, _selected_column_index, data_points_to_interpolate(_selected_row_index, _selected_column_index));
        _attributes[index].patch->UpdateVertexBufferObjectsOfData();
    }

    bool new_image_1 = false;
    bool new_image_2 = false;
    bool new_image_3 = false;

    Direction direction_1, direction_2, direction_3;

    _attributes[index].patch->SetData(r, c, cp);

    MoveControlPoint(index, r, c, new_image_1, direction_1);

    if (r == 0 && c == 0)
    {
        if (_attributes[index].neighbours[NW] != nullptr)
        {
            new_image_1 = true;
            direction_1 = NW;

            if (_attributes[index].connection_type[NW] == NW)
            {
                _attributes[index].neighbours[NW]->patch->SetData(0, 0, cp);

                DCoordinate3 cp01 = (*_attributes[index].patch)(0, 1);
                DCoordinate3 cp10 = (*_attributes[index].patch)(1, 0);

                _attributes[index].neighbours[NW]->patch->SetData(0, 1, 2 * cp - cp01);
                _attributes[index].neighbours[NW]->patch->SetData(1, 0, 2 * cp - cp10);
            }

            if (_attributes[index].connection_type[NW] == NE)
            {
                _attributes[index].neighbours[NW]->patch->SetData(0, 3, cp);

                DCoordinate3 cp01 = (*_attributes[index].patch)(0, 1);
                DCoordinate3 cp10 = (*_attributes[index].patch)(1, 0);

                _attributes[index].neighbours[NW]->patch->SetData(0, 2, 2 * cp - cp01);
                _attributes[index].neighbours[NW]->patch->SetData(1, 3, 2 * cp - cp10);
            }

            if (_attributes[index].connection_type[NW] == SW)
            {
                _attributes[index].neighbours[NW]->patch->SetData(3, 0, cp);

                DCoordinate3 cp01 = (*_attributes[index].patch)(0, 1);
                DCoordinate3 cp10 = (*_attributes[index].patch)(1, 0);

                _attributes[index].neighbours[NW]->patch->SetData(3, 1, 2 * cp - cp01);
                _attributes[index].neighbours[NW]->patch->SetData(2, 0, 2 * cp - cp10);
            }

            if (_attributes[index].connection_type[NW] == SE)
            {
                _attributes[index].neighbours[NW]->patch->SetData(3, 3, cp);

                DCoordinate3 cp01 = (*_attributes[index].patch)(0, 1);
                DCoordinate3 cp10 = (*_attributes[index].patch)(1, 0);

                _attributes[index].neighbours[NW]->patch->SetData(3, 2, 2 * cp - cp01);
                _attributes[index].neighbours[NW]->patch->SetData(2, 3, 2 * cp - cp10);
            }
        }

        if (_attributes[index].neighbours[N] != nullptr)
        {
            new_image_2 = true;
            direction_2 = N;

            if(_attributes[index].connection_type[N] == N)
            {
                 _attributes[index].neighbours[N]->patch->SetData(0, 0, cp);

                 DCoordinate3 cp10 = (*_attributes[index].patch)(1, 0);

                 _attributes[index].neighbours[N]->patch->SetData(1, 0, 2 * cp - cp10);
            }

            if(_attributes[index].connection_type[N] == E)
            {
                 _attributes[index].neighbours[N]->patch->SetData(3, 3, cp);

                 DCoordinate3 cp10 = (*_attributes[index].patch)(1, 0);

                 _attributes[index].neighbours[N]->patch->SetData(3, 2, 2 * cp - cp10);
            }

            if(_attributes[index].connection_type[N] == S)
            {
                 cout << "szia" << endl;
                 _attributes[index].neighbours[N]->patch->SetData(3, 0, cp);

                 DCoordinate3 cp10 = (*_attributes[index].patch)(1, 0);

                 _attributes[index].neighbours[N]->patch->SetData(2, 0, 2 * cp - cp10);
            }

            if(_attributes[index].connection_type[N] == W)
            {
                 _attributes[index].neighbours[N]->patch->SetData(3, 0, cp);

                 DCoordinate3 cp10 = (*_attributes[index].patch)(1, 0);

                 _attributes[index].neighbours[N]->patch->SetData(3, 1, 2 * cp - cp10);
            }
        }

        if (_attributes[index].neighbours[W] != nullptr)
        {
            new_image_3 = true;
            direction_3 = W;

            if(_attributes[index].connection_type[W] == N)
            {
                 _attributes[index].neighbours[W]->patch->SetData(0, 0, cp);

                 DCoordinate3 cp01 = (*_attributes[index].patch)(0, 1);

                 _attributes[index].neighbours[W]->patch->SetData(1, 0, 2 * cp - cp01);
            }

            if(_attributes[index].connection_type[W] == E)
            {
                 _attributes[index].neighbours[W]->patch->SetData(0, 3, cp);

                DCoordinate3 cp01 = (*_attributes[index].patch)(0, 1);

                 _attributes[index].neighbours[W]->patch->SetData(0, 2, 2 * cp - cp01);
            }

            if(_attributes[index].connection_type[W] == S)
            {
                 _attributes[index].neighbours[W]->patch->SetData(3, 3, cp);

                 DCoordinate3 cp01 = (*_attributes[index].patch)(0, 1);

                 _attributes[index].neighbours[W]->patch->SetData(2, 3, 2 * cp - cp01);
            }

            if(_attributes[index].connection_type[W] == W)
            {
                 _attributes[index].neighbours[W]->patch->SetData(0, 0, cp);

                 DCoordinate3 cp01 = (*_attributes[index].patch)(0, 1);

                 _attributes[index].neighbours[W]->patch->SetData(0, 1, 2 * cp - cp01);
            }
        }

    }


    if (r == 0 && c == 3)
    {
        if (_attributes[index].neighbours[NE] != nullptr)
        {
            new_image_1 = true;
            direction_1 = NE;

            if (_attributes[index].connection_type[NE] == NW)
            {
                _attributes[index].neighbours[NE]->patch->SetData(0, 0, cp);

                DCoordinate3 cp02 = (*_attributes[index].patch)(0, 2);
                DCoordinate3 cp13 = (*_attributes[index].patch)(1, 3);

                _attributes[index].neighbours[NE]->patch->SetData(0, 1, 2 * cp - cp02);
                _attributes[index].neighbours[NE]->patch->SetData(1, 0, 2 * cp - cp13);
            }

            if (_attributes[index].connection_type[NE] == NE)
            {
                _attributes[index].neighbours[NE]->patch->SetData(0, 3, cp);

                DCoordinate3 cp02 = (*_attributes[index].patch)(0, 2);
                DCoordinate3 cp13 = (*_attributes[index].patch)(1, 3);

                _attributes[index].neighbours[NE]->patch->SetData(0, 2, 2 * cp - cp02);
                _attributes[index].neighbours[NE]->patch->SetData(1, 3, 2 * cp - cp13);
            }

            if (_attributes[index].connection_type[NE] == SW)
            {
                _attributes[index].neighbours[NE]->patch->SetData(3, 0, cp);

                DCoordinate3 cp02 = (*_attributes[index].patch)(0, 2);
                DCoordinate3 cp13 = (*_attributes[index].patch)(1, 3);

                _attributes[index].neighbours[NE]->patch->SetData(3, 1, 2 * cp - cp02);
                _attributes[index].neighbours[NE]->patch->SetData(2, 0, 2 * cp - cp13);
            }

            if (_attributes[index].connection_type[NE] == SE)
            {
                _attributes[index].neighbours[NE]->patch->SetData(3, 3, cp);

                DCoordinate3 cp02 = (*_attributes[index].patch)(0, 2);
                DCoordinate3 cp13 = (*_attributes[index].patch)(1, 3);

                _attributes[index].neighbours[NE]->patch->SetData(3, 2, 2 * cp - cp02);
                _attributes[index].neighbours[NE]->patch->SetData(2, 3, 2 * cp - cp13);
            }
        }

        if (_attributes[index].neighbours[N] != nullptr)
        {
            new_image_2 = true;
            direction_2 = N;

            if(_attributes[index].connection_type[N] == N)
            {
                 _attributes[index].neighbours[N]->patch->SetData(0, 3, cp);

                 DCoordinate3 cp13 = (*_attributes[index].patch)(1, 3);

                 _attributes[index].neighbours[N]->patch->SetData(1, 3, 2 * cp - cp13);
            }

            if(_attributes[index].connection_type[N] == E)
            {
                 _attributes[index].neighbours[N]->patch->SetData(0, 3, cp);

                 DCoordinate3 cp13 = (*_attributes[index].patch)(1, 3);

                 _attributes[index].neighbours[N]->patch->SetData(0, 2, 2 * cp - cp13);
            }

            if(_attributes[index].connection_type[N] == S)
            {
                 _attributes[index].neighbours[N]->patch->SetData(3, 3, cp);

                 DCoordinate3 cp13 = (*_attributes[index].patch)(1, 3);

                 _attributes[index].neighbours[N]->patch->SetData(2, 3, 2 * cp - cp13);
            }

            if(_attributes[index].connection_type[N] == W)
            {
                 _attributes[index].neighbours[N]->patch->SetData(3, 0, cp);

                 DCoordinate3 cp13 = (*_attributes[index].patch)(1, 3);

                 _attributes[index].neighbours[N]->patch->SetData(3, 1, 2 * cp - cp13);
            }
        }

        if (_attributes[index].neighbours[E] != nullptr)
        {
            new_image_3 = true;
            direction_3 = E;

            if (_attributes[index].connection_type[E] == N)
            {
                _attributes[index].neighbours[E]->patch->SetData(0, 3, cp);

                DCoordinate3 cp02 = (*_attributes[index].patch)(0, 2);

                _attributes[index].neighbours[E]->patch->SetData(1, 3, 2 * cp - cp02);
            }

            if (_attributes[index].connection_type[E] == E)
            {
                _attributes[index].neighbours[E]->patch->SetData(0, 3, cp);

                DCoordinate3 cp02 = (*_attributes[index].patch)(0, 2);

                _attributes[index].neighbours[E]->patch->SetData(0, 2, 2 * cp - cp02);
            }

            if (_attributes[index].connection_type[E] == W)
            {
                _attributes[index].neighbours[E]->patch->SetData(0, 0, cp);

                DCoordinate3 cp02 = (*_attributes[index].patch)(0, 2);

                _attributes[index].neighbours[E]->patch->SetData(0, 1, 2 * cp - cp02);
            }

            if (_attributes[index].connection_type[E] == S)
            {
                _attributes[index].neighbours[E]->patch->SetData(3, 0, cp);

                DCoordinate3 cp02 = (*_attributes[index].patch)(0, 2);

                _attributes[index].neighbours[E]->patch->SetData(2, 0, 2 * cp - cp02);
            }
        }
    }



    if (r == 1 && c == 1)
    {
        MoveControlPoint(index, 0, 1, new_image_1, direction_1);

        MoveControlPoint(index, 1, 0, new_image_2, direction_2);
    }

    if (r == 1 && c == 2)
    {
        MoveControlPoint(index, 0, 2, new_image_1, direction_1);

        MoveControlPoint(index, 1, 3, new_image_2, direction_2);
    }


    if (r == 2 && c == 1)
    {
        MoveControlPoint(index, 2, 0, new_image_1, direction_1);

        MoveControlPoint(index, 3, 1, new_image_2, direction_2);
    }

    if (r == 2 && c == 2)
    {
        MoveControlPoint(index, 2, 3, new_image_1, direction_1);

        MoveControlPoint(index, 3, 2, new_image_2, direction_2);
    }

    if (r == 3 && c == 0)
    {
        if (_attributes[index].neighbours[SW] != nullptr)
        {
            new_image_1 = true;
            direction_1 = SW;

            if (_attributes[index].connection_type[SW] == NW)
            {
                _attributes[index].neighbours[SW]->patch->SetData(0, 0, cp);

                DCoordinate3 cp20 = (*_attributes[index].patch)(2, 0);
                DCoordinate3 cp31 = (*_attributes[index].patch)(3, 1);

                _attributes[index].neighbours[SW]->patch->SetData(0, 1, 2 * cp - cp20);
                _attributes[index].neighbours[SW]->patch->SetData(1, 0, 2 * cp - cp31);
            }

            if (_attributes[index].connection_type[SW] == NE)
            {
                _attributes[index].neighbours[SW]->patch->SetData(0, 3, cp);

                DCoordinate3 cp20 = (*_attributes[index].patch)(2, 0);
                DCoordinate3 cp31 = (*_attributes[index].patch)(3, 1);

                _attributes[index].neighbours[SW]->patch->SetData(0, 2, 2 * cp - cp20);
                _attributes[index].neighbours[SW]->patch->SetData(1, 3, 2 * cp - cp31);
            }

            if (_attributes[index].connection_type[SW] == SW)
            {
                _attributes[index].neighbours[SW]->patch->SetData(3, 0, cp);

                DCoordinate3 cp20 = (*_attributes[index].patch)(2, 0);
                DCoordinate3 cp31 = (*_attributes[index].patch)(3, 1);

                _attributes[index].neighbours[SW]->patch->SetData(3, 1, 2 * cp - cp20);
                _attributes[index].neighbours[SW]->patch->SetData(2, 0, 2 * cp - cp31);
            }

            if (_attributes[index].connection_type[SW] == SE)
            {
                _attributes[index].neighbours[SW]->patch->SetData(3, 3, cp);

                DCoordinate3 cp20 = (*_attributes[index].patch)(2, 0);
                DCoordinate3 cp31 = (*_attributes[index].patch)(3, 1);

                _attributes[index].neighbours[SW]->patch->SetData(3, 2, 2 * cp - cp20);
                _attributes[index].neighbours[SW]->patch->SetData(2, 3, 2 * cp - cp31);
            }
        }

        if (_attributes[index].neighbours[W] != nullptr)
        {
            new_image_2 = true;
            direction_2 = W;

            if(_attributes[index].connection_type[W] == N)
            {
                 _attributes[index].neighbours[W]->patch->SetData(0, 3, cp);

                 DCoordinate3 cp31 = (*_attributes[index].patch)(3, 1);

                 _attributes[index].neighbours[W]->patch->SetData(1, 3, 2 * cp - cp31);
            }

            if(_attributes[index].connection_type[W] == E)
            {
                 _attributes[index].neighbours[W]->patch->SetData(3, 3, cp);

                 DCoordinate3 cp31 = (*_attributes[index].patch)(3, 1);

                 _attributes[index].neighbours[W]->patch->SetData(3, 2, 2 * cp - cp31);
            }

            if(_attributes[index].connection_type[W] == S)
            {
                 _attributes[index].neighbours[W]->patch->SetData(3, 0, cp);

                 DCoordinate3 cp31 = (*_attributes[index].patch)(3, 1);

                 _attributes[index].neighbours[W]->patch->SetData(2, 0, 2 * cp - cp31);
            }

            if(_attributes[index].connection_type[W] == W)
            {
                 _attributes[index].neighbours[W]->patch->SetData(3, 0, cp);

                 DCoordinate3 cp31 = (*_attributes[index].patch)(3, 1);

                 _attributes[index].neighbours[W]->patch->SetData(3, 1, 2 * cp - cp31);
            }
        }

        if (_attributes[index].neighbours[S] != nullptr)
        {
            new_image_3 = true;
            direction_3 = S;

            if (_attributes[index].connection_type[S] == N)
            {
                _attributes[index].neighbours[S]->patch->SetData(0, 0, cp);

                DCoordinate3 cp20 = (*_attributes[index].patch)(2, 0);

                _attributes[index].neighbours[S]->patch->SetData(1, 0, 2 * cp - cp20);
            }

            if (_attributes[index].connection_type[S] == E)
            {
                _attributes[index].neighbours[S]->patch->SetData(0, 3, cp);

                DCoordinate3 cp20 = (*_attributes[index].patch)(2, 0);

                _attributes[index].neighbours[S]->patch->SetData(0, 2, 2 * cp - cp20);
            }

            if (_attributes[index].connection_type[S] == W)
            {
                _attributes[index].neighbours[S]->patch->SetData(3, 0, cp);

                DCoordinate3 cp20 = (*_attributes[index].patch)(2, 0);

                _attributes[index].neighbours[S]->patch->SetData(3, 1, 2 * cp - cp20);
            }

            if (_attributes[index].connection_type[S] == S)
            {
                _attributes[index].neighbours[S]->patch->SetData(3, 0, cp);

                DCoordinate3 cp20 = (*_attributes[index].patch)(2, 0);

                _attributes[index].neighbours[S]->patch->SetData(2, 0, 2 * cp - cp20);
            }
        }
    }


    if (r == 3 && c == 3)
    {
        if (_attributes[index].neighbours[SE] != nullptr)
        {
            new_image_1 = true;
            direction_1 = SE;

            if (_attributes[index].connection_type[SE] == NW)
            {
                _attributes[index].neighbours[SE]->patch->SetData(0, 0, cp);

                DCoordinate3 cp23 = (*_attributes[index].patch)(2, 3);
                DCoordinate3 cp32 = (*_attributes[index].patch)(3, 2);

                _attributes[index].neighbours[SE]->patch->SetData(0, 1, 2 * cp - cp23);
                _attributes[index].neighbours[SE]->patch->SetData(1, 0, 2 * cp - cp32);
            }

            if (_attributes[index].connection_type[SE] == NE)
            {
                _attributes[index].neighbours[SE]->patch->SetData(0, 3, cp);

                DCoordinate3 cp23 = (*_attributes[index].patch)(2, 3);
                DCoordinate3 cp32 = (*_attributes[index].patch)(3, 2);

                _attributes[index].neighbours[SE]->patch->SetData(0, 2, 2 * cp - cp23);
                _attributes[index].neighbours[SE]->patch->SetData(1, 3, 2 * cp - cp32);
            }

            if (_attributes[index].connection_type[SE] == SW)
            {
                _attributes[index].neighbours[SE]->patch->SetData(3, 0, cp);

                DCoordinate3 cp23 = (*_attributes[index].patch)(2, 3);
                DCoordinate3 cp32 = (*_attributes[index].patch)(3, 2);

                _attributes[index].neighbours[SE]->patch->SetData(3, 1, 2 * cp23 - cp);
                _attributes[index].neighbours[SE]->patch->SetData(2, 0, 2 * cp32 - cp);
            }

            if (_attributes[index].connection_type[SE] == SE)
            {
                _attributes[index].neighbours[SE]->patch->SetData(3, 3, cp);

                DCoordinate3 cp23 = (*_attributes[index].patch)(2, 3);
                DCoordinate3 cp32 = (*_attributes[index].patch)(3, 2);

                _attributes[index].neighbours[SE]->patch->SetData(3, 2, 2 * cp - cp23);
                _attributes[index].neighbours[SE]->patch->SetData(2, 3, 2 * cp - cp32);
            }
        }

        if (_attributes[index].neighbours[E] != nullptr)
        {
            new_image_2 = true;
            direction_2 = E;

            if (_attributes[index].connection_type[E] == N)
            {
                _attributes[index].neighbours[E]->patch->SetData(0, 0, cp);

                DCoordinate3 cp32 = (*_attributes[index].patch)(3, 2);

                _attributes[index].neighbours[E]->patch->SetData(1, 0, 2 * cp - cp32);
            }

            if (_attributes[index].connection_type[E] == E)
            {
                _attributes[index].neighbours[E]->patch->SetData(3, 3, cp);

                 DCoordinate3 cp32 = (*_attributes[index].patch)(3, 2);

                _attributes[index].neighbours[E]->patch->SetData(3, 2, 2 * cp - cp32);
            }

            if (_attributes[index].connection_type[E] == W)
            {
                _attributes[index].neighbours[E]->patch->SetData(3, 0, cp);

                 DCoordinate3 cp32 = (*_attributes[index].patch)(3, 2);

                _attributes[index].neighbours[E]->patch->SetData(3, 1, 2 * cp - cp32);
            }

            if (_attributes[index].connection_type[E] == S)
            {
                _attributes[index].neighbours[E]->patch->SetData(3, 3, cp);

                 DCoordinate3 cp32 = (*_attributes[index].patch)(3, 2);

                _attributes[index].neighbours[E]->patch->SetData(2, 3, 2 * cp - cp32);
            }
        }

        if (_attributes[index].neighbours[S] != nullptr)
        {
            new_image_3 = true;
            direction_3 = S;

            if (_attributes[index].connection_type[S] == N)
            {
                _attributes[index].neighbours[S]->patch->SetData(0, 3, cp);

                DCoordinate3 cp23 = (*_attributes[index].patch)(2, 3);

                _attributes[index].neighbours[S]->patch->SetData(1, 3, 2 * cp - cp23);
            }

            if (_attributes[index].connection_type[S] == E)
            {
                _attributes[index].neighbours[S]->patch->SetData(3, 3, cp);

                DCoordinate3 cp23 = (*_attributes[index].patch)(2, 3);

                _attributes[index].neighbours[S]->patch->SetData(3, 2, 2 * cp - cp23);
            }

            if (_attributes[index].connection_type[S] == W)
            {
                _attributes[index].neighbours[S]->patch->SetData(0, 0, cp);

                DCoordinate3 cp23 = (*_attributes[index].patch)(2, 3);

                _attributes[index].neighbours[S]->patch->SetData(0, 1, 2 * cp - cp23);
            }

            if (_attributes[index].connection_type[S] == S)
            {
                _attributes[index].neighbours[S]->patch->SetData(3, 3, cp);

                DCoordinate3 cp23 = (*_attributes[index].patch)(2, 3);

                _attributes[index].neighbours[S]->patch->SetData(2, 3, 2 * cp - cp23);
            }
        }
    }

    _attributes[index].image = _attributes[index].patch->GenerateImage(_uDivPointCount, _vDivPointCount, GL_STATIC_DRAW);

    if (_attributes[index].image)
        _attributes[index].image->UpdateVertexBufferObjects();

    _attributes[index]._uIsoparametricLines = _attributes[index].patch->GenerateUIsoparametricLines(_uIsoLineCount, 2, _uDivPointCount);
    _attributes[index]._vIsoparametricLines = _attributes[index].patch->GenerateVIsoparametricLines(_vIsoLineCount, 2, _vDivPointCount);

    for (GLuint i = 0; i < _attributes[index]._uIsoparametricLines->GetColumnCount(); i++)
    {
        (*_attributes[index]._uIsoparametricLines)[i]->UpdateVertexBufferObjects();
    }

    for (GLuint i = 0; i < _attributes[index]._vIsoparametricLines->GetColumnCount(); i++)
    {
        (*_attributes[index]._vIsoparametricLines)[i]->UpdateVertexBufferObjects();
    }

    //_patch.GetData(_selected_row_index, _selected_column_index, data_points_to_interpolate(_selected_row_index, _selected_column_index));
    _attributes[index].patch->UpdateVertexBufferObjectsOfData();

    if (new_image_1)
    {
        _attributes[index].neighbours[direction_1]->image = _attributes[index].neighbours[direction_1]->patch->GenerateImage(_uDivPointCount, _vDivPointCount, GL_STATIC_DRAW);

        if (_attributes[index].neighbours[direction_1]->image)
            _attributes[index].neighbours[direction_1]->image->UpdateVertexBufferObjects();

        _attributes[index].neighbours[direction_1]->_uIsoparametricLines = _attributes[index].neighbours[direction_1]->patch->GenerateUIsoparametricLines(_uIsoLineCount, 2, _uDivPointCount);
        _attributes[index].neighbours[direction_1]->_vIsoparametricLines = _attributes[index].neighbours[direction_1]->patch->GenerateVIsoparametricLines(_vIsoLineCount, 2, _vDivPointCount);

        for (GLuint i = 0; i < _attributes[index].neighbours[direction_1]->_uIsoparametricLines->GetColumnCount(); i++)
        {
            (*_attributes[index].neighbours[direction_1]->_uIsoparametricLines)[i]->UpdateVertexBufferObjects();
        }

        for (GLuint i = 0; i < _attributes[index].neighbours[direction_1]->_vIsoparametricLines->GetColumnCount(); i++)
        {
            (*_attributes[index].neighbours[direction_1]->_vIsoparametricLines)[i]->UpdateVertexBufferObjects();
        }

        //_patch.GetData(_selected_row_index, _selected_column_index, data_points_to_interpolate(_selected_row_index, _selected_column_index));
        _attributes[index].neighbours[direction_1]->patch->UpdateVertexBufferObjectsOfData();

    }

    if (new_image_2)
    {
        _attributes[index].neighbours[direction_2]->image = _attributes[index].neighbours[direction_2]->patch->GenerateImage(_uDivPointCount, _vDivPointCount, GL_STATIC_DRAW);

        if (_attributes[index].neighbours[direction_2]->image)
            _attributes[index].neighbours[direction_2]->image->UpdateVertexBufferObjects();

        _attributes[index].neighbours[direction_2]->_uIsoparametricLines = _attributes[index].neighbours[direction_2]->patch->GenerateUIsoparametricLines(_uIsoLineCount, 2, _uDivPointCount);
        _attributes[index].neighbours[direction_2]->_vIsoparametricLines = _attributes[index].neighbours[direction_2]->patch->GenerateVIsoparametricLines(_vIsoLineCount, 2, _vDivPointCount);

        for (GLuint i = 0; i < _attributes[index].neighbours[direction_2]->_uIsoparametricLines->GetColumnCount(); i++)
        {
            (*_attributes[index].neighbours[direction_2]->_uIsoparametricLines)[i]->UpdateVertexBufferObjects();
        }

        for (GLuint i = 0; i < _attributes[index].neighbours[direction_2]->_vIsoparametricLines->GetColumnCount(); i++)
        {
            (*_attributes[index].neighbours[direction_2]->_vIsoparametricLines)[i]->UpdateVertexBufferObjects();
        }

        //_patch.GetData(_selected_row_index, _selected_column_index, data_points_to_interpolate(_selected_row_index, _selected_column_index));
        _attributes[index].neighbours[direction_2]->patch->UpdateVertexBufferObjectsOfData();

    }

    if (new_image_3)
    {
        _attributes[index].neighbours[direction_3]->image = _attributes[index].neighbours[direction_3]->patch->GenerateImage(_uDivPointCount, _vDivPointCount, GL_STATIC_DRAW);

        if (_attributes[index].neighbours[direction_3]->image)
            _attributes[index].neighbours[direction_3]->image->UpdateVertexBufferObjects();

        _attributes[index].neighbours[direction_3]->_uIsoparametricLines = _attributes[index].neighbours[direction_3]->patch->GenerateUIsoparametricLines(_uIsoLineCount, 2, _uDivPointCount);
        _attributes[index].neighbours[direction_3]->_vIsoparametricLines = _attributes[index].neighbours[direction_3]->patch->GenerateVIsoparametricLines(_vIsoLineCount, 2, _vDivPointCount);

        for (GLuint i = 0; i < _attributes[index].neighbours[direction_3]->_uIsoparametricLines->GetColumnCount(); i++)
        {
            (*_attributes[index].neighbours[direction_3]->_uIsoparametricLines)[i]->UpdateVertexBufferObjects();
        }

        for (GLuint i = 0; i < _attributes[index].neighbours[direction_3]->_vIsoparametricLines->GetColumnCount(); i++)
        {
            (*_attributes[index].neighbours[direction_3]->_vIsoparametricLines)[i]->UpdateVertexBufferObjects();
        }

        //_patch.GetData(_selected_row_index, _selected_column_index, data_points_to_interpolate(_selected_row_index, _selected_column_index));
        _attributes[index].neighbours[direction_3]->patch->UpdateVertexBufferObjectsOfData();

    }

}

GLdouble FirstOrderAlgebraicTrigonometricCompositePatch3::GetAlphaU() const
{
    return _alpha[0];
}

GLdouble FirstOrderAlgebraicTrigonometricCompositePatch3::GetAlphaV() const
{
    return _alpha[1];
}

GLboolean FirstOrderAlgebraicTrigonometricCompositePatch3::SetAlphaOfPatches(int index)
{
    for (GLuint i = 0; i < _patch_count; ++i)
    {
        if (index == 0)
        {
            _attributes[i].patch->setAlphaU(_alpha[index]);
        }

        if (index == 1)
        {
            _attributes[i].patch->setAlphaV(_alpha[index]);
        }

    }
    return GL_TRUE;
}

GLboolean FirstOrderAlgebraicTrigonometricCompositePatch3::SetAlphaHelper(int i)
{
    if (_attributes[i].image)
    {
       delete _attributes[i].image; _attributes[i].image = nullptr;
    }

   _attributes[i].image = _attributes[i].patch->GenerateImage(_uDivPointCount, _vDivPointCount);


   if (!_attributes[i].image || !_attributes[i].image->UpdateVertexBufferObjects())
   {
       return false;
   }

   _attributes[i].patch->UpdateVertexBufferObjectsOfData();

   if (_attributes[i]._uIsoparametricLines)
   {
       for (GLuint j = 0; j < _attributes[i]._uIsoparametricLines->GetColumnCount(); ++j){
           delete (* _attributes[i]._uIsoparametricLines)[j];
       }
       delete _attributes[i]._uIsoparametricLines;
   }

   if (_attributes[i]._vIsoparametricLines)
   {
       for (GLuint j = 0; j < _attributes[i]._vIsoparametricLines->GetColumnCount(); ++j){
           delete (* _attributes[i]._vIsoparametricLines)[j];
       }
       delete _attributes[i]._vIsoparametricLines;
   }

   _attributes[i]._uIsoparametricLines = _attributes[i].patch->GenerateUIsoparametricLines(_uIsoLineCount, 2, _uDivPointCount);

   if (_attributes[i]._uIsoparametricLines)
   {
       for (GLuint j = 0; j < _attributes[i]._uIsoparametricLines->GetColumnCount(); ++j){
           (* _attributes[i]._uIsoparametricLines)[j]->UpdateVertexBufferObjects();
       }
   }

   _attributes[i]._vIsoparametricLines = _attributes[i].patch->GenerateVIsoparametricLines(_vIsoLineCount, 2, _vDivPointCount);
   if (_attributes[i]._vIsoparametricLines)
   {
       for (GLuint j = 0; j < _attributes[i]._vIsoparametricLines->GetColumnCount(); ++j){
           (* _attributes[i]._vIsoparametricLines)[j]->UpdateVertexBufferObjects();
       }
   }

   return true;
}

GLboolean FirstOrderAlgebraicTrigonometricCompositePatch3::SetAlphaU(double value)
{
    _alpha[0] = value;

    for (GLuint i = 0; i < _patch_count; ++i)
    {
        if (!PatchExists(i))
        {
            continue;
        }

        _attributes[i].patch->setAlphaU(_alpha[0]);
        if (! SetAlphaHelper(i))
        {
            return false;
        }
    }

    return true;
}

GLboolean FirstOrderAlgebraicTrigonometricCompositePatch3::SetAlphaV(double value)
{
    _alpha[1] = value;

    for (GLuint i = 0; i < _patch_count; ++i)
    {
        if (!PatchExists(i))
        {
            continue;
        }

        _attributes[i].patch->setAlphaV(_alpha[1]);
        if (! SetAlphaHelper(i))
        {
            return false;
        }
    }

    return true;
}

DCoordinate3 FirstOrderAlgebraicTrigonometricCompositePatch3::GetSelectedPatchSelectedControlPoint(GLuint selected_foat_patch, GLuint row_index, GLuint column_index)
{
    return _attributes[selected_foat_patch].patch->operator()(row_index,column_index);
}

void FirstOrderAlgebraicTrigonometricCompositePatch3::ModifyPositionOfSelectedPatch(GLuint selected_foat_patch, GLuint row_index, GLuint column_index, GLdouble x, GLdouble y, GLdouble z)
{
    try
    {
        if (_attributes[selected_foat_patch].patch)
        {
            (*_attributes[selected_foat_patch].patch)(row_index, column_index)[0] = x;
            (*_attributes[selected_foat_patch].patch)(row_index, column_index)[1] = y;
            (*_attributes[selected_foat_patch].patch)(row_index, column_index)[2] = z;

            delete _attributes[selected_foat_patch].image; _attributes[selected_foat_patch].image = nullptr;
            _attributes[selected_foat_patch].image = _attributes[selected_foat_patch].patch->GenerateImage(_uDivPointCount, _vDivPointCount);

            if (!_attributes[selected_foat_patch].image)
            {
                DeleteExistingPatch(selected_foat_patch);
                throw Exception("Error occured during the image creation of the arc!");

            }

            if (!_attributes[selected_foat_patch].image->UpdateVertexBufferObjects())
            {
                DeleteExistingPatch(selected_foat_patch);
                throw Exception("Error occured during the update of the VBOs of the arc!");
            }

            _attributes[selected_foat_patch]._uIsoparametricLines = _attributes[selected_foat_patch].patch->GenerateUIsoparametricLines(_uIsoLineCount, 2, _uDivPointCount);
            _attributes[selected_foat_patch]._vIsoparametricLines = _attributes[selected_foat_patch].patch->GenerateVIsoparametricLines(_vIsoLineCount, 2, _vDivPointCount);

            for (GLuint i = 0; i < _attributes[selected_foat_patch]._uIsoparametricLines->GetColumnCount(); i++)
            {
                (*_attributes[selected_foat_patch]._uIsoparametricLines)[i]->UpdateVertexBufferObjects();
            }

            for (GLuint i = 0; i < _attributes[selected_foat_patch]._vIsoparametricLines->GetColumnCount(); i++)
            {
                (*_attributes[selected_foat_patch]._vIsoparametricLines)[i]->UpdateVertexBufferObjects();
            }

            if (!_attributes[selected_foat_patch].patch->UpdateVertexBufferObjectsOfData())
            {
                DeleteExistingPatch(selected_foat_patch);
                throw Exception("Error occured during the update of the VBOs of the arc!");
            }
        }
    }
    catch (Exception &e)
    {
        cout << e << endl;
    }
}


GLboolean FirstOrderAlgebraicTrigonometricCompositePatch3::JoinExistingPatches(GLuint index_0, Direction direction_0, GLuint index_1, Direction direction_1)
{
    //N, NW, W, SW, S, SE, E, NE
    assert((direction_0 == N || direction_0 == NW || direction_0 == W || direction_0 == SW || direction_0 == S || direction_0 == SE || direction_0 == E || direction_0 == NE) &&
           (direction_1 == N || direction_1 == NW || direction_1 == W || direction_1 == SW || direction_1 == S || direction_1 == SE || direction_1 == E || direction_1 == NE) &&
           index_0 >= 0 && index_0 <= _patch_count && index_1 >= 0 && index_1 <= _patch_count);

    if (_attributes[index_0].neighbours[direction_0] != nullptr || _attributes[index_1].neighbours[direction_1] != nullptr)
    {
        cout << "Nem kothetek ossze osszekotottet!\n";
        return false;
    }

    _attributes[_patch_count].patch = new (nothrow) FirstOrderAlgebraicTrigonometricPatch3(_alpha[0], _alpha[1]);
    _attributes[_patch_count].material = &MatFBPearl;

    if (direction_0 == N)
    {
        for (int i = 0; i < 4; i++)
        {
            DCoordinate3 cp;
            (*_attributes[index_0].patch).GetData(0, i, cp);
            (*_attributes[_patch_count].patch).SetData(3, i, cp);
        }

        for (int i = 0; i < 4; i++)
        {
            DCoordinate3 cp0, cp1;
            (*_attributes[index_0].patch).GetData(0, i, cp0);
            (*_attributes[index_0].patch).GetData(1, i, cp1);
            (*_attributes[_patch_count].patch).SetData(2, i, 2 * cp0 - cp1);
        }

        if (direction_1 == N)
        {
            for (int i = 0; i < 4; i++)
            {
                DCoordinate3 cp;
                (*_attributes[index_1].patch).GetData(0, i, cp);
                (*_attributes[_patch_count].patch).SetData(0, i, cp);
            }

            for (int i = 0; i < 4; i++)
            {
                DCoordinate3 cp0, cp1;
                (*_attributes[index_1].patch).GetData(0, i, cp0);
                (*_attributes[index_1].patch).GetData(1, i, cp1);
                (*_attributes[_patch_count].patch).SetData(1, i, 2 * cp0 - cp1);
            }
        }

        if (direction_1 == E)
        {
            for (int i = 0; i < 4; i++)
            {
                DCoordinate3 cp;
                (*_attributes[index_1].patch).GetData(3 - i, 3, cp);
                (*_attributes[_patch_count].patch).SetData(0, i, cp);
            }

            for (int i = 0; i < 4; i++)
            {
                DCoordinate3 cp2, cp3;
                (*_attributes[index_1].patch).GetData(3 - i, 2, cp2);
                (*_attributes[index_1].patch).GetData(3 - i, 3, cp3);
                (*_attributes[_patch_count].patch).SetData(1, i, 2 * cp3 - cp2);
            }
        }

        if (direction_1 == S)
        {
            for (int i = 0; i < 4; i++)
            {
                DCoordinate3 cp;
                (*_attributes[index_1].patch).GetData(3, i, cp);
                (*_attributes[_patch_count].patch).SetData(0, i, cp);
            }

            for (int i = 0; i < 4; i++)
            {
                DCoordinate3 cp2, cp3;
                (*_attributes[index_1].patch).GetData(2, i, cp2);
                (*_attributes[index_1].patch).GetData(3, i, cp3);
                (*_attributes[_patch_count].patch).SetData(1, i, 2 * cp3 - cp2);
            }
        }

        if (direction_1 == W)
        {
            for (int i = 0; i < 4; i++)
            {
                DCoordinate3 cp;
                (*_attributes[index_1].patch).GetData(i, 0, cp);
                cout << 0 << " " << cp << endl;
                (*_attributes[_patch_count].patch).SetData(0, i, cp);
            }

            for (int i = 0; i < 4; i++)
            {
                DCoordinate3 cp0, cp1;
                (*_attributes[index_1].patch).GetData(i, 0, cp0);
                (*_attributes[index_1].patch).GetData(i, 1, cp1);
                cout << 0 << " " << cp0<< endl;
                cout << 1 << " " << cp1 << endl;
                (*_attributes[_patch_count].patch).SetData(1, i, 2 * cp0 - cp1);
            }
        }
    }

    if (direction_0 == E)
    {
        for (int i = 0; i < 4; i++)
        {
            DCoordinate3 cp;
            (*_attributes[index_0].patch).GetData(i, 3, cp);
            (*_attributes[_patch_count].patch).SetData(i, 0, cp);
        }

        for (int i = 0; i < 4; i++)
        {
            DCoordinate3 cp2, cp3;
            (*_attributes[index_0].patch).GetData(i, 2, cp2);
            (*_attributes[index_0].patch).GetData(i, 3, cp3);
            (*_attributes[_patch_count].patch).SetData(i, 1, 2 * cp3 - cp2);
        }

        if (direction_1 == N)
        {
            for (int i = 0; i < 4; i++)
            {
                DCoordinate3 cp;
                (*_attributes[index_1].patch).GetData(0, i, cp);
                (*_attributes[_patch_count].patch).SetData(i, 3, cp);
            }

            for (int i = 0; i < 4; i++)
            {
                DCoordinate3 cp0, cp1;
                (*_attributes[index_1].patch).GetData(0, i, cp0);
                (*_attributes[index_1].patch).GetData(1, i, cp1);
                (*_attributes[_patch_count].patch).SetData(i, 2, 2 * cp0 - cp1);
            }
        }

        if (direction_1 == E)
        {
            for (int i = 0; i < 4; i++)
            {
                DCoordinate3 cp;
                (*_attributes[index_1].patch).GetData(i, 3, cp);
                (*_attributes[_patch_count].patch).SetData(i, 3, cp);
            }

            for (int i = 0; i < 4; i++)
            {
                DCoordinate3 cp2, cp3;
                (*_attributes[index_1].patch).GetData(i, 2, cp2);
                (*_attributes[index_1].patch).GetData(i, 3, cp3);
                (*_attributes[_patch_count].patch).SetData(i, 2, 2 * cp3 - cp2);
            }
        }

        if (direction_1 == S)
        {
            for (int i = 0; i < 4; i++)
            {
                DCoordinate3 cp;
                (*_attributes[index_1].patch).GetData(3, i, cp);
                (*_attributes[_patch_count].patch).SetData(i, 3, cp);
            }

            for (int i = 0; i < 4; i++)
            {
                DCoordinate3 cp2, cp3;
                (*_attributes[index_1].patch).GetData(2, i, cp2);
                (*_attributes[index_1].patch).GetData(3, i, cp3);
                (*_attributes[_patch_count].patch).SetData(i, 2, 2 * cp3 - cp2);
            }
        }

        if (direction_1 == W)
        {
            for (int i = 0; i < 4; i++)
            {
                DCoordinate3 cp;
                (*_attributes[index_1].patch).GetData(i, 0, cp);
                cout << 0 << " " << cp << endl;
                (*_attributes[_patch_count].patch).SetData(i, 3, cp);
            }

            for (int i = 0; i < 4; i++)
            {
                DCoordinate3 cp0, cp1;
                (*_attributes[index_1].patch).GetData(i, 0, cp0);
                (*_attributes[index_1].patch).GetData(i, 1, cp1);
                cout << 0 << " " << cp0<< endl;
                cout << 1 << " " << cp1 << endl;
                (*_attributes[_patch_count].patch).SetData(i, 2, 2 * cp0 - cp1);
            }
        }
     }

    if (direction_0 == S)
    {
        for (int i = 0; i < 4; i++)
        {
            DCoordinate3 cp;
            (*_attributes[index_0].patch).GetData(3, i, cp);
            (*_attributes[_patch_count].patch).SetData(0, i, cp);
        }

        for (int i = 0; i < 4; i++)
        {
            DCoordinate3 cp2, cp3;
            (*_attributes[index_0].patch).GetData(2, i, cp2);
            (*_attributes[index_0].patch).GetData(3, i, cp3);
            (*_attributes[_patch_count].patch).SetData(1, i, 2 * cp3 - cp2);
        }

        if (direction_1 == N)
        {
            for (int i = 0; i < 4; i++)
            {
                DCoordinate3 cp;
                (*_attributes[index_1].patch).GetData(0, i, cp);
                (*_attributes[_patch_count].patch).SetData(3, i, cp);
            }

            for (int i = 0; i < 4; i++)
            {
                DCoordinate3 cp0, cp1;
                (*_attributes[index_1].patch).GetData(0, i, cp0);
                (*_attributes[index_1].patch).GetData(1, i, cp1);
                (*_attributes[_patch_count].patch).SetData(2, i, 2 * cp0 - cp1);
            }
        }

        if (direction_1 == E)
        {
            for (int i = 0; i < 4; i++)
            {
                DCoordinate3 cp;
                (*_attributes[index_1].patch).GetData(i, 3, cp);
                (*_attributes[_patch_count].patch).SetData(3, i, cp);
            }

            for (int i = 0; i < 4; i++)
            {
                DCoordinate3 cp2, cp3;
                (*_attributes[index_1].patch).GetData(i, 2, cp2);
                (*_attributes[index_1].patch).GetData(i, 3, cp3);
                (*_attributes[_patch_count].patch).SetData(2, i, 2 * cp3 - cp2);
            }
        }

        if (direction_1 == S)
        {
            for (int i = 0; i < 4; i++)
            {
                DCoordinate3 cp;
                (*_attributes[index_1].patch).GetData(3, i, cp);
                (*_attributes[_patch_count].patch).SetData(3, i, cp);
            }

            for (int i = 0; i < 4; i++)
            {
                DCoordinate3 cp2, cp3;
                (*_attributes[index_1].patch).GetData(2, i, cp2);
                (*_attributes[index_1].patch).GetData(3, i, cp3);
                (*_attributes[_patch_count].patch).SetData(2, i, 2 * cp3 - cp2);
            }
        }

        if (direction_1 == W)
        {
            for (int i = 0; i < 4; i++)
            {
                DCoordinate3 cp;
                (*_attributes[index_1].patch).GetData(3 - i, 0, cp);
                (*_attributes[_patch_count].patch).SetData(3, i, cp);
            }

            for (int i = 0; i < 4; i++)
            {
                DCoordinate3 cp0, cp1;
                (*_attributes[index_1].patch).GetData(3 - i, 0, cp0);
                (*_attributes[index_1].patch).GetData(3 - i, 1, cp1);
                (*_attributes[_patch_count].patch).SetData(2, i, 2 * cp0 - cp1);
            }
        }
    }

    if (direction_0 == W)
    {
        for (int i = 0; i < 4; i++)
        {
            DCoordinate3 cp;
            (*_attributes[index_0].patch).GetData(i, 0, cp);
            (*_attributes[_patch_count].patch).SetData(i, 3, cp);
        }

        for (int i = 0; i < 4; i++)
        {
            DCoordinate3 cp0, cp1;
            (*_attributes[index_0].patch).GetData(i, 0, cp0);
            (*_attributes[index_0].patch).GetData(i, 1, cp1);
            (*_attributes[_patch_count].patch).SetData(i, 2, 2 * cp0 - cp1);
        }

        if (direction_1 == N)
        {
            for (int i = 0; i < 4; i++)
            {
                DCoordinate3 cp;
                (*_attributes[index_1].patch).GetData(0, i, cp);
                (*_attributes[_patch_count].patch).SetData(i, 0, cp);
            }

            for (int i = 0; i < 4; i++)
            {
                DCoordinate3 cp0, cp1;
                (*_attributes[index_1].patch).GetData(0, i, cp0);
                (*_attributes[index_1].patch).GetData(1, i, cp1);
                (*_attributes[_patch_count].patch).SetData(i, 1, 2 * cp0 - cp1);
            }
        }

        if (direction_1 == E)
        {
            for (int i = 0; i < 4; i++)
            {
                DCoordinate3 cp;
                (*_attributes[index_1].patch).GetData(i, 3, cp);
                (*_attributes[_patch_count].patch).SetData(i, 0, cp);
            }

            for (int i = 0; i < 4; i++)
            {
                DCoordinate3 cp2, cp3;
                (*_attributes[index_1].patch).GetData(i, 2, cp2);
                (*_attributes[index_1].patch).GetData(i, 3, cp3);
                (*_attributes[_patch_count].patch).SetData(i, 1, 2 * cp3 - cp2);
            }
        }

        if (direction_1 == S)
        {
            for (int i = 0; i < 4; i++)
            {
                DCoordinate3 cp;
                (*_attributes[index_1].patch).GetData(3, i, cp);
                (*_attributes[_patch_count].patch).SetData(i, 0, cp);
            }

            for (int i = 0; i < 4; i++)
            {
                DCoordinate3 cp2, cp3;
                (*_attributes[index_1].patch).GetData(2, i, cp2);
                (*_attributes[index_1].patch).GetData(3, i, cp3);
                (*_attributes[_patch_count].patch).SetData(i, 1, 2 * cp3 - cp2);
            }
        }

        if (direction_1 == W)
        {
            for (int i = 0; i < 4; i++)
            {
                DCoordinate3 cp;
                (*_attributes[index_1].patch).GetData(i, 0, cp);
                (*_attributes[_patch_count].patch).SetData(i, 0, cp);
            }

            for (int i = 0; i < 4; i++)
            {
                DCoordinate3 cp0, cp1;
                (*_attributes[index_1].patch).GetData(i, 0, cp0);
                (*_attributes[index_1].patch).GetData(i, 1, cp1);
                (*_attributes[_patch_count].patch).SetData(i, 1, 2 * cp0 - cp1);
            }
        }
    }

    if (direction_0 == NE)
    {
        DCoordinate3 cp03, cp02, cp13;
        (*_attributes[index_0].patch).GetData(0, 3, cp03);
        (*_attributes[index_0].patch).GetData(0, 2, cp02);
        (*_attributes[index_0].patch).GetData(1, 3, cp13);
        for(int i = 0; i < 3; i++)
        {
            (*_attributes[_patch_count].patch).SetData(3, i, cp03 + i * (cp03 - cp02));
            (*_attributes[_patch_count].patch).SetData(3 - i, 0, cp03 + i * (cp03 - cp13));
        }

        DCoordinate3 fekete00, piros00, fekete33, piros33;
        fekete00 = cp03 + 3 * (cp03 - cp13);
        piros33 = cp03 + 3 * (cp03 - cp02);

        if (direction_1 == SW)
        {
            DCoordinate3 cp30, cp20, cp31;
            (*_attributes[index_1].patch).GetData(3, 0, cp30);
            (*_attributes[index_1].patch).GetData(2, 0, cp20);
            (*_attributes[index_1].patch).GetData(3, 1, cp31);
            for(int i = 0; i < 3; i++)
            {
                (*_attributes[_patch_count].patch).SetData(0, 3 - i, cp30 + i * (cp30 - cp31));
                (*_attributes[_patch_count].patch).SetData(i, 3, cp30 + i * (cp30 - cp20));
            }

            fekete33 = cp30 + 3 * (cp30 - cp20);
            piros00 = cp30 + 3 * (cp30 - cp31);
        }

        if (direction_1 == NW)
        {
            DCoordinate3 cp00, cp10, cp01;
            (*_attributes[index_1].patch).GetData(0, 0, cp00);
            (*_attributes[index_1].patch).GetData(1, 0, cp10);
            (*_attributes[index_1].patch).GetData(0, 1, cp01);
            for(int i = 0; i < 3; i++)
            {
                (*_attributes[_patch_count].patch).SetData(0, 3 - i, cp00 + i * (cp00 - cp01));
                (*_attributes[_patch_count].patch).SetData(i, 3, cp00 + i * (cp10 - cp00));
            }

            fekete33 = cp00 + 3 * (cp10 - cp00);
            piros00 = cp00 + 3 * (cp00 - cp10);
        }

        if (direction_1 == SE)
        {
            DCoordinate3 cp33, cp23, cp32;
            (*_attributes[index_1].patch).GetData(3, 3, cp33);
            (*_attributes[index_1].patch).GetData(2, 3, cp23);
            (*_attributes[index_1].patch).GetData(3, 2, cp32);
            for(int i = 0; i < 3; i++)
            {
                (*_attributes[_patch_count].patch).SetData(0, 3 - i, cp33 + i * (cp32 - cp33));
                (*_attributes[_patch_count].patch).SetData(i, 3, cp33 + i * (cp33 - cp23));
            }

            fekete33 = cp33 + 3 * (cp33 - cp32);
            piros00 = cp33 + 3 * (cp32 - cp33);
        }

        if (direction_1 == NE)
        {
            DCoordinate3 cp03, cp02, cp13;
            (*_attributes[index_1].patch).GetData(0, 3, cp03);
            (*_attributes[index_1].patch).GetData(0, 2, cp02);
            (*_attributes[index_1].patch).GetData(1, 3, cp13);
            for(int i = 0; i < 3; i++)
            {
                (*_attributes[_patch_count].patch).SetData(0, 3 - i, cp03 + i * (cp02 - cp03));
                (*_attributes[_patch_count].patch).SetData(i, 3, cp03 + i * (cp13 - cp03));
            }

            fekete00 = cp03 + 3 * (cp02 - cp03);
            piros33 = cp03 + 3 * (cp13 - cp03);
        }

        (*_attributes[_patch_count].patch).SetData(0, 0, 0.5 * (fekete00 + piros00));
        (*_attributes[_patch_count].patch).SetData(3, 3, 0.5 * (fekete33 + piros33));

        (*_attributes[_patch_count].patch)(1, 1) = 0.5 * ((*_attributes[_patch_count].patch)(1, 0) + 1 / 3 * ((*_attributes[_patch_count].patch)(1, 3) - (*_attributes[_patch_count].patch)(1, 0)) + (*_attributes[_patch_count].patch)(0, 1) + 1 / 3 * ((*_attributes[_patch_count].patch)(3, 1) - (*_attributes[_patch_count].patch)(0, 1)));
        (*_attributes[_patch_count].patch)(1, 2) = 0.5 * ((*_attributes[_patch_count].patch)(1, 0) + 2 / 3 * ((*_attributes[_patch_count].patch)(1, 3) - (*_attributes[_patch_count].patch)(1, 0)) + (*_attributes[_patch_count].patch)(0, 2) + 1 / 3 * ((*_attributes[_patch_count].patch)(3, 2) - (*_attributes[_patch_count].patch)(0, 2)));
        (*_attributes[_patch_count].patch)(2, 1) = 0.5 * ((*_attributes[_patch_count].patch)(2, 0) + 1 / 3 * ((*_attributes[_patch_count].patch)(2, 3) - (*_attributes[_patch_count].patch)(2, 0)) + (*_attributes[_patch_count].patch)(0, 1) + 1 / 3 * ((*_attributes[_patch_count].patch)(3, 1) - (*_attributes[_patch_count].patch)(0, 1)));
        (*_attributes[_patch_count].patch)(2, 2) = 0.5 * ((*_attributes[_patch_count].patch)(2, 0) + 2 / 3 * ((*_attributes[_patch_count].patch)(2, 3) - (*_attributes[_patch_count].patch)(2, 0)) + (*_attributes[_patch_count].patch)(0, 1) + 2 / 3 * ((*_attributes[_patch_count].patch)(3, 2) - (*_attributes[_patch_count].patch)(0, 2)));

    }

    if (direction_0 == SW)
    {
        DCoordinate3 cp30, cp20, cp31;
        (*_attributes[index_0].patch).GetData(3, 0, cp30);
        (*_attributes[index_0].patch).GetData(2, 0, cp20);
        (*_attributes[index_0].patch).GetData(3, 1, cp31);
        for(int i = 0; i < 3; i++)
        {
            (*_attributes[_patch_count].patch).SetData(0, 3 - i, cp30 + i * (cp30 - cp31));
            (*_attributes[_patch_count].patch).SetData(i, 3, cp30 + i * (cp30 - cp20));
        }

        DCoordinate3 fekete00, piros00, fekete33, piros33;
        fekete33 = cp30 + 3 * (cp30 - cp20);
        piros00 = cp30 + 3 * (cp30 - cp31);

        if (direction_1 == NE)
        {
            DCoordinate3 cp03, cp02, cp13;
            (*_attributes[index_1].patch).GetData(0, 3, cp03);
            (*_attributes[index_1].patch).GetData(0, 2, cp02);
            (*_attributes[index_1].patch).GetData(1, 3, cp13);
            for(int i = 0; i < 3; i++)
            {
                (*_attributes[_patch_count].patch).SetData(3, i, cp03 + i * (cp03 - cp02));
                (*_attributes[_patch_count].patch).SetData(3 - i, 0, cp03 + i * (cp03 - cp13));
            }

            fekete00 = cp03 + 3 * (cp03 - cp13);
            piros33 = cp03 + 3 * (cp03 - cp02);
        }

        if (direction_1 == SW)
        {
            DCoordinate3 cp30, cp20, cp31;
            (*_attributes[index_1].patch).GetData(3, 0, cp30);
            (*_attributes[index_1].patch).GetData(2, 0, cp20);
            (*_attributes[index_1].patch).GetData(3, 1, cp31);
            for(int i = 0; i < 3; i++)
            {
                (*_attributes[_patch_count].patch).SetData(3 - i, 0, cp30 + i * (cp20 - cp30));
                (*_attributes[_patch_count].patch).SetData(3, i, cp30 + i * (cp31 - cp30));
            }

            fekete33 = cp30 + 3 * (cp31 - cp30);
            piros00 = cp30 + 3 * (cp20 - cp30);
        }

        if (direction_1 == NW)
        {
            DCoordinate3 cp00, cp10, cp01;
            (*_attributes[index_1].patch).GetData(0, 0, cp00);
            (*_attributes[index_1].patch).GetData(1, 0, cp10);
            (*_attributes[index_1].patch).GetData(0, 1, cp01);
            for(int i = 0; i < 3; i++)
            {
                (*_attributes[_patch_count].patch).SetData(3 - i, 0, cp00 + i * (cp00 - cp10));
                (*_attributes[_patch_count].patch).SetData(3, i, cp00 + i * (cp01 - cp00));
            }

            fekete33 = cp00 + 3 * (cp01 - cp00);
            piros00 = cp00 + 3 * (cp00 - cp10);
        }

        if (direction_1 == SE)
        {
            DCoordinate3 cp33, cp23, cp32;
            (*_attributes[index_1].patch).GetData(3, 3, cp33);
            (*_attributes[index_1].patch).GetData(2, 3, cp23);
            (*_attributes[index_1].patch).GetData(3, 2, cp32);
            for(int i = 0; i < 3; i++)
            {
                (*_attributes[_patch_count].patch).SetData(3 - i, 0, cp33 + i * (cp23 - cp33));
                (*_attributes[_patch_count].patch).SetData(3, i, cp33 + i * (cp33 - cp32));
            }

            fekete33 = cp33 + 3 * (cp33 - cp32);
            piros00 = cp33 + 3 * (cp23 - cp33);
        }

        (*_attributes[_patch_count].patch).SetData(0, 0, 0.5 * (fekete00 + piros00));
        (*_attributes[_patch_count].patch).SetData(3, 3, 0.5 * (fekete33 + piros33));

        (*_attributes[_patch_count].patch)(1, 1) = 0.5 * ((*_attributes[_patch_count].patch)(1, 0) + 1 / 3 * ((*_attributes[_patch_count].patch)(1, 3) - (*_attributes[_patch_count].patch)(1, 0)) + (*_attributes[_patch_count].patch)(0, 1) + 1 / 3 * ((*_attributes[_patch_count].patch)(3, 1) - (*_attributes[_patch_count].patch)(0, 1)));
        (*_attributes[_patch_count].patch)(1, 2) = 0.5 * ((*_attributes[_patch_count].patch)(1, 0) + 2 / 3 * ((*_attributes[_patch_count].patch)(1, 3) - (*_attributes[_patch_count].patch)(1, 0)) + (*_attributes[_patch_count].patch)(0, 2) + 1 / 3 * ((*_attributes[_patch_count].patch)(3, 2) - (*_attributes[_patch_count].patch)(0, 2)));
        (*_attributes[_patch_count].patch)(2, 1) = 0.5 * ((*_attributes[_patch_count].patch)(2, 0) + 1 / 3 * ((*_attributes[_patch_count].patch)(2, 3) - (*_attributes[_patch_count].patch)(2, 0)) + (*_attributes[_patch_count].patch)(0, 1) + 1 / 3 * ((*_attributes[_patch_count].patch)(3, 1) - (*_attributes[_patch_count].patch)(0, 1)));
        (*_attributes[_patch_count].patch)(2, 2) = 0.5 * ((*_attributes[_patch_count].patch)(2, 0) + 2 / 3 * ((*_attributes[_patch_count].patch)(2, 3) - (*_attributes[_patch_count].patch)(2, 0)) + (*_attributes[_patch_count].patch)(0, 1) + 2 / 3 * ((*_attributes[_patch_count].patch)(3, 2) - (*_attributes[_patch_count].patch)(0, 2)));

    }

    if (direction_0 == NW)
    {
        DCoordinate3 cp00, cp01, cp10;
        (*_attributes[index_0].patch).GetData(0, 0, cp00);
        (*_attributes[index_0].patch).GetData(0, 1, cp01);
        (*_attributes[index_0].patch).GetData(1, 0, cp10);
        for(int i = 0; i < 3; i++)
        {
            (*_attributes[_patch_count].patch).SetData(3, 3 - i, cp00 + i * (cp00 - cp01));
            (*_attributes[_patch_count].patch).SetData(3 - i, 3, cp00 + i * (cp00 - cp10));
        }

        DCoordinate3 fekete30, piros30, fekete03, piros03;
        fekete30 = cp00 + 3 * (cp00 - cp01);
        piros03 = cp00 + 3 * (cp00 - cp10);

        if (direction_1 == SE)
        {
            DCoordinate3 cp33, cp23, cp32;
            (*_attributes[index_1].patch).GetData(3, 3, cp33);
            (*_attributes[index_1].patch).GetData(2, 3, cp23);
            (*_attributes[index_1].patch).GetData(3, 2, cp32);
            for(int i = 0; i < 3; i++)
            {
                (*_attributes[_patch_count].patch).SetData(0, i, cp33 + i * (cp33 - cp32));
                (*_attributes[_patch_count].patch).SetData(i, 0, cp33 + i * (cp33 - cp23));
            }

            fekete03 = cp33 + 3 * (cp33 - cp32);
            piros30 = cp33 + 3 * (cp33 - cp23);
        }

        if (direction_1 == NW)
        {
            DCoordinate3 cp00, cp01, cp10;
            (*_attributes[index_1].patch).GetData(0, 0, cp00);
            (*_attributes[index_1].patch).GetData(0, 1, cp01);
            (*_attributes[index_1].patch).GetData(1, 0, cp10);
            for(int i = 0; i < 3; i++)
            {
                (*_attributes[_patch_count].patch).SetData(0, i, cp00 + i * (cp01 - cp00));
                (*_attributes[_patch_count].patch).SetData(i, 0, cp00 + i * (cp10 - cp00));
            }

            fekete30 = cp00 + 3 * (cp10 - cp00);
            piros03 = cp00 + 3 * (cp01 - cp00);
        }

        if (direction_1 == NE)
        {
            DCoordinate3 cp03, cp02, cp13;
            (*_attributes[index_1].patch).GetData(0, 3, cp03);
            (*_attributes[index_1].patch).GetData(0, 2, cp02);
            (*_attributes[index_1].patch).GetData(1, 3, cp13);
            for(int i = 0; i < 3; i++)
            {
                (*_attributes[_patch_count].patch).SetData(0, i, cp03 + i * (cp03 - cp02));
                (*_attributes[_patch_count].patch).SetData(i, 3, cp03 + i * (cp13 - cp03));
            }

            fekete30 = cp03 + 3 * (cp13 - cp03);
            piros03 = cp03 + 3 * (cp03 - cp02);
        }

        if (direction_1 == SW)
        {
            DCoordinate3 cp30, cp20, cp31;
            (*_attributes[index_1].patch).GetData(3, 0, cp30);
            (*_attributes[index_1].patch).GetData(2, 0, cp20);
            (*_attributes[index_1].patch).GetData(3, 1, cp31);
            for(int i = 0; i < 3; i++)
            {
                (*_attributes[_patch_count].patch).SetData(0, i, cp30 + i * (cp31 - cp30));
                (*_attributes[_patch_count].patch).SetData(i, 3, cp30 + i * (cp30 - cp20));
            }

            fekete30 = cp30 + 3 * (cp30 - cp20);
            piros03 = cp30 + 3 * (cp31 - cp30);
        }

        (*_attributes[_patch_count].patch).SetData(3, 0, 0.5 * (fekete30 + piros30));
        (*_attributes[_patch_count].patch).SetData(0, 3, 0.5 * (fekete03 + piros03));

        (*_attributes[_patch_count].patch)(1, 1) = 0.5 * ((*_attributes[_patch_count].patch)(1, 0) + 1 / 3 * ((*_attributes[_patch_count].patch)(1, 3) - (*_attributes[_patch_count].patch)(1, 0)) + (*_attributes[_patch_count].patch)(0, 1) + 1 / 3 * ((*_attributes[_patch_count].patch)(3, 1) - (*_attributes[_patch_count].patch)(0, 1)));
        (*_attributes[_patch_count].patch)(1, 2) = 0.5 * ((*_attributes[_patch_count].patch)(1, 0) + 2 / 3 * ((*_attributes[_patch_count].patch)(1, 3) - (*_attributes[_patch_count].patch)(1, 0)) + (*_attributes[_patch_count].patch)(0, 2) + 1 / 3 * ((*_attributes[_patch_count].patch)(3, 2) - (*_attributes[_patch_count].patch)(0, 2)));
        (*_attributes[_patch_count].patch)(2, 1) = 0.5 * ((*_attributes[_patch_count].patch)(2, 0) + 1 / 3 * ((*_attributes[_patch_count].patch)(2, 3) - (*_attributes[_patch_count].patch)(2, 0)) + (*_attributes[_patch_count].patch)(0, 1) + 1 / 3 * ((*_attributes[_patch_count].patch)(3, 1) - (*_attributes[_patch_count].patch)(0, 1)));
        (*_attributes[_patch_count].patch)(2, 2) = 0.5 * ((*_attributes[_patch_count].patch)(2, 0) + 2 / 3 * ((*_attributes[_patch_count].patch)(2, 3) - (*_attributes[_patch_count].patch)(2, 0)) + (*_attributes[_patch_count].patch)(0, 1) + 2 / 3 * ((*_attributes[_patch_count].patch)(3, 2) - (*_attributes[_patch_count].patch)(0, 2)));

    }

    if (direction_0 == SE)
    {
        DCoordinate3 cp33, cp23, cp32;
        (*_attributes[index_0].patch).GetData(3, 3, cp33);
        (*_attributes[index_0].patch).GetData(2, 3, cp23);
        (*_attributes[index_0].patch).GetData(3, 2, cp32);
        for(int i = 0; i < 3; i++)
        {
            (*_attributes[_patch_count].patch).SetData(0, i, cp33 + i * (cp33 - cp32));
            (*_attributes[_patch_count].patch).SetData(i, 0, cp33 + i * (cp33 - cp23));
        }

        DCoordinate3 fekete30, piros30, fekete03, piros03;
        fekete03 = cp33 + 3 * (cp33 - cp32);
        piros30 = cp33 + 3 * (cp33 - cp23);

        if (direction_1 == NW)
        {
            DCoordinate3 cp00, cp01, cp10;
            (*_attributes[index_1].patch).GetData(0, 0, cp00);
            (*_attributes[index_1].patch).GetData(0, 1, cp01);
            (*_attributes[index_1].patch).GetData(1, 0, cp10);
            for(int i = 0; i < 3; i++)
            {
                (*_attributes[_patch_count].patch).SetData(3, 3 - i, cp00 + i * (cp00 - cp01));
                (*_attributes[_patch_count].patch).SetData(3 - i, 3, cp00 + i * (cp00 - cp10));
            }

            DCoordinate3 fekete30, piros30, fekete03, piros03;
            fekete30 = cp00 + 3 * (cp00 - cp01);
            piros03 = cp00 + 3 * (cp00 - cp10);
        }

        if (direction_1 == NE)
        {
            DCoordinate3 cp03, cp02, cp13;
            (*_attributes[index_1].patch).GetData(0, 3, cp03);
            (*_attributes[index_1].patch).GetData(0, 2, cp02);
            (*_attributes[index_1].patch).GetData(1, 3, cp13);
            for(int i = 0; i < 3; i++)
            {
                (*_attributes[_patch_count].patch).SetData(3, 3 - i, cp03 + i * (cp02 - cp03));
                (*_attributes[_patch_count].patch).SetData(3 - i, 3, cp03 + i * (cp03 - cp13));
            }

            fekete30 = cp03 + 3 * (cp02 - cp03);
            piros03 = cp03 + 3 * (cp03 - cp13);
        }

        if (direction_1 == SE)
        {
            DCoordinate3 cp33, cp23, cp32;
            (*_attributes[index_1].patch).GetData(3, 3, cp33);
            (*_attributes[index_1].patch).GetData(2, 3, cp23);
            (*_attributes[index_1].patch).GetData(3, 2, cp32);
            for(int i = 0; i < 3; i++)
            {
                (*_attributes[_patch_count].patch).SetData(3, 3 - i, cp33 + i * (cp32 - cp33));
                (*_attributes[_patch_count].patch).SetData(3 - i, 3, cp33 + i * (cp23 - cp33));
            }

            fekete03 = cp33 + 3 * (cp23 - cp33);
            piros30 = cp33 + 3 * (cp32 - cp33);
        }

        if (direction_1 == SW)
        {
            DCoordinate3 cp30, cp20, cp31;
            (*_attributes[index_1].patch).GetData(3, 0, cp30);
            (*_attributes[index_1].patch).GetData(2, 0, cp20);
            (*_attributes[index_1].patch).GetData(3, 1, cp31);
            for(int i = 0; i < 3; i++)
            {
                (*_attributes[_patch_count].patch).SetData(3, 3 - i, cp30 + i * (cp30 - cp31));
                (*_attributes[_patch_count].patch).SetData(3 - i, 3, cp30 + i * (cp20 - cp30));
            }

            fekete30 = cp30 + 3 * (cp30 - cp31);
            piros03 = cp30 + 3 * (cp20 - cp30);
        }

        (*_attributes[_patch_count].patch).SetData(3, 0, 0.5 * (fekete30 + piros30));
        (*_attributes[_patch_count].patch).SetData(0, 3, 0.5 * (fekete03 + piros03));

        (*_attributes[_patch_count].patch)(1, 1) = 0.5 * ((*_attributes[_patch_count].patch)(1, 0) + 1 / 3 * ((*_attributes[_patch_count].patch)(1, 3) - (*_attributes[_patch_count].patch)(1, 0)) + (*_attributes[_patch_count].patch)(0, 1) + 1 / 3 * ((*_attributes[_patch_count].patch)(3, 1) - (*_attributes[_patch_count].patch)(0, 1)));
        (*_attributes[_patch_count].patch)(1, 2) = 0.5 * ((*_attributes[_patch_count].patch)(1, 0) + 2 / 3 * ((*_attributes[_patch_count].patch)(1, 3) - (*_attributes[_patch_count].patch)(1, 0)) + (*_attributes[_patch_count].patch)(0, 2) + 1 / 3 * ((*_attributes[_patch_count].patch)(3, 2) - (*_attributes[_patch_count].patch)(0, 2)));
        (*_attributes[_patch_count].patch)(2, 1) = 0.5 * ((*_attributes[_patch_count].patch)(2, 0) + 1 / 3 * ((*_attributes[_patch_count].patch)(2, 3) - (*_attributes[_patch_count].patch)(2, 0)) + (*_attributes[_patch_count].patch)(0, 1) + 1 / 3 * ((*_attributes[_patch_count].patch)(3, 1) - (*_attributes[_patch_count].patch)(0, 1)));
        (*_attributes[_patch_count].patch)(2, 2) = 0.5 * ((*_attributes[_patch_count].patch)(2, 0) + 2 / 3 * ((*_attributes[_patch_count].patch)(2, 3) - (*_attributes[_patch_count].patch)(2, 0)) + (*_attributes[_patch_count].patch)(0, 1) + 2 / 3 * ((*_attributes[_patch_count].patch)(3, 2) - (*_attributes[_patch_count].patch)(0, 2)));

    }

    _attributes[_patch_count].neighbours[direction_0] = &_attributes[index_0];
    _attributes[_patch_count].neighbours[direction_1] = &_attributes[index_1];
    _attributes[_patch_count].connection_type[direction_0] = direction_0; // modosit!!!
    _attributes[_patch_count].connection_type[direction_1] = direction_1; // modosit!!!

    _attributes[index_0].neighbours[direction_0] = &_attributes[_patch_count];
    _attributes[index_0].connection_type[direction_0] = direction_1;

    _attributes[index_1].neighbours[direction_1] = &_attributes[_patch_count];
    _attributes[index_1].connection_type[direction_1] = direction_0;

    _attributes[_patch_count].patch->UpdateVertexBufferObjectsOfData();

    _attributes[_patch_count].image = (*_attributes[_patch_count].patch).GenerateImage(_uDivPointCount, _vDivPointCount, GL_STATIC_DRAW);

    if (_attributes[_patch_count].image)
        _attributes[_patch_count].image->UpdateVertexBufferObjects();

    (*_attributes[_patch_count].patch).UpdateVertexBufferObjectsOfData();

    _attributes[_patch_count]._uIsoparametricLines = (*_attributes[_patch_count].patch).GenerateUIsoparametricLines(_uIsoLineCount, 2, _uDivPointCount);
    _attributes[_patch_count]._vIsoparametricLines = (*_attributes[_patch_count].patch).GenerateVIsoparametricLines(_vIsoLineCount, 2, _vDivPointCount);

    for (GLuint i = 0; i < _attributes[_patch_count]._uIsoparametricLines->GetColumnCount(); i++)
    {
        (*_attributes[_patch_count]._uIsoparametricLines)[i]->UpdateVertexBufferObjects();
    }

    for (GLuint i = 0; i < _attributes[_patch_count]._vIsoparametricLines->GetColumnCount(); i++)
    {
        (*_attributes[_patch_count]._vIsoparametricLines)[i]->UpdateVertexBufferObjects();
    }

    _patch_count++;
    return true;

}


GLboolean FirstOrderAlgebraicTrigonometricCompositePatch3::MergeExistingPatches(GLuint index_0, Direction direction_0, GLuint index_1, Direction direction_1)
{
    assert((direction_0 == N || direction_0 == W || direction_0 == E || direction_0 == S || direction_0 == E || direction_0 == NW || direction_0 == NE || direction_0 == SE || direction_0 == SW)
           && (direction_1 == N || direction_1 == W || direction_1 == E || direction_1 == S || direction_1 == E || direction_1 == NW || direction_1 == NE || direction_1 == SE || direction_1 == SW));

    if (_attributes[index_0].neighbours[direction_0] != nullptr || _attributes[index_1].neighbours[direction_1] != nullptr)
    {
        return GL_FALSE;
    }

     DCoordinate3 centre1, centre2, centre3, centre4;

    if (direction_0 == E)
    {

        if (direction_1 == E)
        {
            centre1 = ((*_attributes[index_0].patch)(0,2) + (*_attributes[index_1].patch)(0,2)) / 2;
            centre2 = ((*_attributes[index_0].patch)(1,2) + (*_attributes[index_1].patch)(1,2)) / 2;
            centre3 = ((*_attributes[index_0].patch)(2,2) + (*_attributes[index_1].patch)(2,2)) / 2;
            centre4 = ((*_attributes[index_0].patch)(3,2) + (*_attributes[index_1].patch)(3,2)) / 2;

            (*_attributes[index_0].patch)(0,3) = centre1;
            (*_attributes[index_0].patch)(1,3) = centre2;
            (*_attributes[index_0].patch)(2,3) = centre3;
            (*_attributes[index_0].patch)(3,3) = centre4;

            (*_attributes[index_1].patch)(0,3) = centre1;
            (*_attributes[index_1].patch)(1,3) = centre2;
            (*_attributes[index_1].patch)(2,3) = centre3;
            (*_attributes[index_1].patch)(3,3) = centre4;

            _attributes[index_0].neighbours[E] = &_attributes[index_1];
            _attributes[index_0].connection_type[E] = E;
            _attributes[index_1].neighbours[E] = &_attributes[index_0];
            _attributes[index_1].connection_type[E] = E;

            return GenerateImageOfSelectedPatches(index_0, index_1);
        }

        if (direction_1 == W)
        {
            centre1 = ((*_attributes[index_0].patch)(0,2) + (*_attributes[index_1].patch)(0,1)) / 2;
            centre2 = ((*_attributes[index_0].patch)(1,2) + (*_attributes[index_1].patch)(1,1)) / 2;
            centre3 = ((*_attributes[index_0].patch)(2,2) + (*_attributes[index_1].patch)(2,1)) / 2;
            centre4 = ((*_attributes[index_0].patch)(3,2) + (*_attributes[index_1].patch)(3,1)) / 2;

            (*_attributes[index_0].patch)(0,3) = centre1;
            (*_attributes[index_0].patch)(1,3) = centre2;
            (*_attributes[index_0].patch)(2,3) = centre3;
            (*_attributes[index_0].patch)(3,3) = centre4;

            (*_attributes[index_1].patch)(0,0) = centre1;
            (*_attributes[index_1].patch)(1,0) = centre2;
            (*_attributes[index_1].patch)(2,0) = centre3;
            (*_attributes[index_1].patch)(3,0) = centre4;

            _attributes[index_0].neighbours[E] = &_attributes[index_1];
            _attributes[index_0].connection_type[E] = W;
            _attributes[index_1].neighbours[W] = &_attributes[index_0];
            _attributes[index_1].connection_type[W] = E;

            return GenerateImageOfSelectedPatches(index_0, index_1);
        }

        if (direction_1 == N)
        {
            centre1 = ((*_attributes[index_0].patch)(3,2) + (*_attributes[index_1].patch)(1,0)) / 2;
            centre2 = ((*_attributes[index_0].patch)(2,2) + (*_attributes[index_1].patch)(1,1)) / 2;
            centre3 = ((*_attributes[index_0].patch)(1,2) + (*_attributes[index_1].patch)(1,2)) / 2;
            centre4 = ((*_attributes[index_0].patch)(0,2) + (*_attributes[index_1].patch)(1,3)) / 2;

            (*_attributes[index_0].patch)(0,3) = centre4;
            (*_attributes[index_0].patch)(1,3) = centre3;
            (*_attributes[index_0].patch)(2,3) = centre2;
            (*_attributes[index_0].patch)(3,3) = centre1;

            (*_attributes[index_1].patch)(0,0) = centre1;
            (*_attributes[index_1].patch)(0,1) = centre2;
            (*_attributes[index_1].patch)(0,2) = centre3;
            (*_attributes[index_1].patch)(0,3) = centre4;

            _attributes[index_0].neighbours[E] = &_attributes[index_1];
            _attributes[index_0].connection_type[E] = N;
            _attributes[index_1].neighbours[N] = &_attributes[index_0];
            _attributes[index_1].connection_type[N] = E;


            return GenerateImageOfSelectedPatches(index_0, index_1);
        }

        if (direction_1 == S)
        {
            centre1 = ((*_attributes[index_0].patch)(0,2) + (*_attributes[index_1].patch)(2,0)) / 2;
            centre2 = ((*_attributes[index_0].patch)(1,2) + (*_attributes[index_1].patch)(2,1)) / 2;
            centre3 = ((*_attributes[index_0].patch)(2,2) + (*_attributes[index_1].patch)(2,2)) / 2;
            centre4 = ((*_attributes[index_0].patch)(3,2) + (*_attributes[index_1].patch)(2,3)) / 2;

            (*_attributes[index_0].patch)(0,3) = centre1;
            (*_attributes[index_0].patch)(1,3) = centre2;
            (*_attributes[index_0].patch)(2,3) = centre3;
            (*_attributes[index_0].patch)(3,3) = centre4;

            (*_attributes[index_1].patch)(3,0) = centre1;
            (*_attributes[index_1].patch)(3,1) = centre2;
            (*_attributes[index_1].patch)(3,2) = centre3;
            (*_attributes[index_1].patch)(3,3) = centre4;

            _attributes[index_0].neighbours[E] = &_attributes[index_1];
            _attributes[index_0].connection_type[E] = S;
            _attributes[index_1].neighbours[S] = &_attributes[index_0];
            _attributes[index_1].connection_type[S] = E;


            return GenerateImageOfSelectedPatches(index_0, index_1);
        }
    }

    if (direction_0 == W)
    {
        if (direction_1 == E)
        {
            centre1 = ((*_attributes[index_0].patch)(0,1) + (*_attributes[index_1].patch)(0,2)) / 2;
            centre2 = ((*_attributes[index_0].patch)(1,1) + (*_attributes[index_1].patch)(1,2)) / 2;
            centre3 = ((*_attributes[index_0].patch)(2,1) + (*_attributes[index_1].patch)(2,2)) / 2;
            centre4 = ((*_attributes[index_0].patch)(3,1) + (*_attributes[index_1].patch)(3,2)) / 2;

            (*_attributes[index_0].patch)(0,0) = centre1;
            (*_attributes[index_0].patch)(1,0) = centre2;
            (*_attributes[index_0].patch)(2,0) = centre3;
            (*_attributes[index_0].patch)(3,0) = centre4;

            (*_attributes[index_1].patch)(0,3) = centre1;
            (*_attributes[index_1].patch)(1,3) = centre2;
            (*_attributes[index_1].patch)(2,3) = centre3;
            (*_attributes[index_1].patch)(3,3) = centre4;

            _attributes[index_0].neighbours[W] = &_attributes[index_1];
            _attributes[index_0].connection_type[W] = E;
            _attributes[index_1].neighbours[E] = &_attributes[index_0];
            _attributes[index_1].connection_type[E] = W;

            return GenerateImageOfSelectedPatches(index_0, index_1);
        }

        if (direction_1 == W)
        {
            centre1 = ((*_attributes[index_0].patch)(0,1) + (*_attributes[index_1].patch)(0,1)) / 2;
            centre2 = ((*_attributes[index_0].patch)(1,1) + (*_attributes[index_1].patch)(1,1)) / 2;
            centre3 = ((*_attributes[index_0].patch)(2,1) + (*_attributes[index_1].patch)(2,1)) / 2;
            centre4 = ((*_attributes[index_0].patch)(3,1) + (*_attributes[index_1].patch)(3,1)) / 2;

            (*_attributes[index_0].patch)(0,0) = centre1;
            (*_attributes[index_0].patch)(1,0) = centre2;
            (*_attributes[index_0].patch)(2,0) = centre3;
            (*_attributes[index_0].patch)(3,0) = centre4;

            (*_attributes[index_1].patch)(0,0) = centre1;
            (*_attributes[index_1].patch)(1,0) = centre2;
            (*_attributes[index_1].patch)(2,0) = centre3;
            (*_attributes[index_1].patch)(3,0) = centre4;

            _attributes[index_0].neighbours[W] = &_attributes[index_1];
            _attributes[index_0].connection_type[W] = W;
            _attributes[index_1].neighbours[W] = &_attributes[index_0];
            _attributes[index_1].connection_type[W] = W;

            return GenerateImageOfSelectedPatches(index_0, index_1);
        }

        if (direction_1 == S)
        {
            centre1 = ((*_attributes[index_0].patch)(0,1) + (*_attributes[index_1].patch)(2,3)) / 2;
            centre2 = ((*_attributes[index_0].patch)(1,1) + (*_attributes[index_1].patch)(2,2)) / 2;
            centre3 = ((*_attributes[index_0].patch)(2,1) + (*_attributes[index_1].patch)(2,1)) / 2;
            centre4 = ((*_attributes[index_0].patch)(3,1) + (*_attributes[index_1].patch)(2,0)) / 2;

            (*_attributes[index_0].patch)(0,0) = centre1;
            (*_attributes[index_0].patch)(1,0) = centre2;
            (*_attributes[index_0].patch)(2,0) = centre3;
            (*_attributes[index_0].patch)(3,0) = centre4;

            (*_attributes[index_1].patch)(3,0) = centre4;
            (*_attributes[index_1].patch)(3,1) = centre3;
            (*_attributes[index_1].patch)(3,2) = centre2;
            (*_attributes[index_1].patch)(3,3) = centre1;

            _attributes[index_0].neighbours[W] = &_attributes[index_1];
            _attributes[index_0].connection_type[W] = S;
            _attributes[index_1].neighbours[S] = &_attributes[index_0];
            _attributes[index_1].connection_type[S] = W;


            return GenerateImageOfSelectedPatches(index_0, index_1);
        }

        if (direction_1 == N)
        {
            centre1 = ((*_attributes[index_0].patch)(0,1) + (*_attributes[index_1].patch)(1,0)) / 2;
            centre2 = ((*_attributes[index_0].patch)(1,1) + (*_attributes[index_1].patch)(1,1)) / 2;
            centre3 = ((*_attributes[index_0].patch)(2,1) + (*_attributes[index_1].patch)(1,2)) / 2;
            centre4 = ((*_attributes[index_0].patch)(3,1) + (*_attributes[index_1].patch)(1,3)) / 2;

            (*_attributes[index_0].patch)(0,0) = centre1;
            (*_attributes[index_0].patch)(1,0) = centre2;
            (*_attributes[index_0].patch)(2,0) = centre3;
            (*_attributes[index_0].patch)(3,0) = centre4;

            (*_attributes[index_1].patch)(0,0) = centre1;
            (*_attributes[index_1].patch)(0,1) = centre2;
            (*_attributes[index_1].patch)(0,2) = centre3;
            (*_attributes[index_1].patch)(0,3) = centre4;

            _attributes[index_0].neighbours[W] = &_attributes[index_1];
            _attributes[index_0].connection_type[W] = N;
            _attributes[index_1].neighbours[N] = &_attributes[index_0];
            _attributes[index_1].connection_type[N] = W;


            return GenerateImageOfSelectedPatches(index_0, index_1);
        }
    }

    if (direction_0 == S)
    {
        if (direction_1 == E)
        {
            centre1 = ((*_attributes[index_0].patch)(2,0) + (*_attributes[index_1].patch)(0,2)) / 2;
            centre2 = ((*_attributes[index_0].patch)(2,1) + (*_attributes[index_1].patch)(1,2)) / 2;
            centre3 = ((*_attributes[index_0].patch)(2,2) + (*_attributes[index_1].patch)(2,2)) / 2;
            centre4 = ((*_attributes[index_0].patch)(2,3) + (*_attributes[index_1].patch)(3,2)) / 2;

            (*_attributes[index_0].patch)(3,0) = centre1;
            (*_attributes[index_0].patch)(3,1) = centre2;
            (*_attributes[index_0].patch)(3,2) = centre3;
            (*_attributes[index_0].patch)(3,3) = centre4;

            (*_attributes[index_1].patch)(0,3) = centre1;
            (*_attributes[index_1].patch)(1,3) = centre2;
            (*_attributes[index_1].patch)(2,3) = centre3;
            (*_attributes[index_1].patch)(3,3) = centre4;

            _attributes[index_0].neighbours[S] = &_attributes[index_1];
            _attributes[index_0].connection_type[S] = E;
            _attributes[index_1].neighbours[E] = &_attributes[index_0];
            _attributes[index_1].connection_type[E] = S;

            return GenerateImageOfSelectedPatches(index_0, index_1);
        }

        if (direction_1 == W)
        {
            centre1 = ((*_attributes[index_0].patch)(2,3) + (*_attributes[index_1].patch)(0,1)) / 2;
            centre2 = ((*_attributes[index_0].patch)(2,2) + (*_attributes[index_1].patch)(1,1)) / 2;
            centre3 = ((*_attributes[index_0].patch)(2,1) + (*_attributes[index_1].patch)(2,1)) / 2;
            centre4 = ((*_attributes[index_0].patch)(2,0) + (*_attributes[index_1].patch)(3,1)) / 2;

            (*_attributes[index_0].patch)(3,0) = centre4;
            (*_attributes[index_0].patch)(3,1) = centre3;
            (*_attributes[index_0].patch)(3,2) = centre2;
            (*_attributes[index_0].patch)(3,3) = centre1;

            (*_attributes[index_1].patch)(0,0) = centre1;
            (*_attributes[index_1].patch)(1,0) = centre2;
            (*_attributes[index_1].patch)(2,0) = centre3;
            (*_attributes[index_1].patch)(3,0) = centre4;

            _attributes[index_0].neighbours[S] = &_attributes[index_1];
            _attributes[index_0].connection_type[S] = W;
            _attributes[index_1].neighbours[W] = &_attributes[index_0];
            _attributes[index_1].connection_type[W] = S;

            return GenerateImageOfSelectedPatches(index_0, index_1);
        }

        if (direction_1 == N)
        {
            centre1 = ((*_attributes[index_0].patch)(2,0) + (*_attributes[index_1].patch)(1,0)) / 2;
            centre2 = ((*_attributes[index_0].patch)(2,1) + (*_attributes[index_1].patch)(1,1)) / 2;
            centre3 = ((*_attributes[index_0].patch)(2,2) + (*_attributes[index_1].patch)(1,2)) / 2;
            centre4 = ((*_attributes[index_0].patch)(2,3) + (*_attributes[index_1].patch)(1,3)) / 2;

            (*_attributes[index_0].patch)(3,0) = centre1;
            (*_attributes[index_0].patch)(3,1) = centre2;
            (*_attributes[index_0].patch)(3,2) = centre3;
            (*_attributes[index_0].patch)(3,3) = centre4;

            (*_attributes[index_1].patch)(0,0) = centre1;
            (*_attributes[index_1].patch)(0,1) = centre2;
            (*_attributes[index_1].patch)(0,2) = centre3;
            (*_attributes[index_1].patch)(0,3) = centre4;

            _attributes[index_0].neighbours[S] = &_attributes[index_1];
            _attributes[index_0].connection_type[S] = N;
            _attributes[index_1].neighbours[N] = &_attributes[index_0];
            _attributes[index_1].connection_type[N] = S;


            return GenerateImageOfSelectedPatches(index_0, index_1);
        }

        if (direction_1 == S)
        {
            centre1 = ((*_attributes[index_0].patch)(2,0) + (*_attributes[index_1].patch)(2,0)) / 2;
            centre2 = ((*_attributes[index_0].patch)(2,1) + (*_attributes[index_1].patch)(2,1)) / 2;
            centre3 = ((*_attributes[index_0].patch)(2,2) + (*_attributes[index_1].patch)(2,2)) / 2;
            centre4 = ((*_attributes[index_0].patch)(2,3) + (*_attributes[index_1].patch)(2,3)) / 2;

            (*_attributes[index_0].patch)(3,0) = centre1;
            (*_attributes[index_0].patch)(3,1) = centre2;
            (*_attributes[index_0].patch)(3,2) = centre3;
            (*_attributes[index_0].patch)(3,3) = centre4;

            (*_attributes[index_1].patch)(3,0) = centre1;
            (*_attributes[index_1].patch)(3,1) = centre2;
            (*_attributes[index_1].patch)(3,2) = centre3;
            (*_attributes[index_1].patch)(3,3) = centre4;

            _attributes[index_0].neighbours[S] = &_attributes[index_1];
            _attributes[index_0].connection_type[S] = S;
            _attributes[index_1].neighbours[S] = &_attributes[index_0];
            _attributes[index_1].connection_type[S] = S;


            return GenerateImageOfSelectedPatches(index_0, index_1);
        }
    }

    if (direction_0 == N)
    {
        if (direction_1 == E)
        {
            centre1 = ((*_attributes[index_0].patch)(1,0) + (*_attributes[index_1].patch)(3,2)) / 2;
            centre2 = ((*_attributes[index_0].patch)(1,1) + (*_attributes[index_1].patch)(2,2)) / 2;
            centre3 = ((*_attributes[index_0].patch)(1,2) + (*_attributes[index_1].patch)(1,2)) / 2;
            centre4 = ((*_attributes[index_0].patch)(1,3) + (*_attributes[index_1].patch)(0,2)) / 2;

            (*_attributes[index_0].patch)(0,0) = centre1;
            (*_attributes[index_0].patch)(0,1) = centre2;
            (*_attributes[index_0].patch)(0,2) = centre3;
            (*_attributes[index_0].patch)(0,3) = centre4;

            (*_attributes[index_1].patch)(0,3) = centre4;
            (*_attributes[index_1].patch)(1,3) = centre3;
            (*_attributes[index_1].patch)(2,3) = centre2;
            (*_attributes[index_1].patch)(3,3) = centre1;

            _attributes[index_0].neighbours[N] = &_attributes[index_1];
            _attributes[index_0].connection_type[N] = E;
            _attributes[index_1].neighbours[E] = &_attributes[index_0];
            _attributes[index_1].connection_type[E] = N;

            return GenerateImageOfSelectedPatches(index_0, index_1);
        }

        if (direction_1 == W)
        {
            centre1 = ((*_attributes[index_0].patch)(1,0) + (*_attributes[index_1].patch)(0,1)) / 2;
            centre2 = ((*_attributes[index_0].patch)(1,1) + (*_attributes[index_1].patch)(1,1)) / 2;
            centre3 = ((*_attributes[index_0].patch)(1,2) + (*_attributes[index_1].patch)(2,1)) / 2;
            centre4 = ((*_attributes[index_0].patch)(1,3) + (*_attributes[index_1].patch)(3,1)) / 2;

            (*_attributes[index_0].patch)(0,0) = centre1;
            (*_attributes[index_0].patch)(0,1) = centre2;
            (*_attributes[index_0].patch)(0,2) = centre3;
            (*_attributes[index_0].patch)(0,3) = centre4;

            (*_attributes[index_1].patch)(0,0) = centre1;
            (*_attributes[index_1].patch)(1,0) = centre2;
            (*_attributes[index_1].patch)(2,0) = centre3;
            (*_attributes[index_1].patch)(3,0) = centre4;

            _attributes[index_0].neighbours[N] = &_attributes[index_1];
            _attributes[index_0].connection_type[N] = W;
            _attributes[index_1].neighbours[W] = &_attributes[index_0];
            _attributes[index_1].connection_type[W] = N;

            return GenerateImageOfSelectedPatches(index_0, index_1);
        }

        if (direction_1 == N)
        {
            centre1 = ((*_attributes[index_0].patch)(1,0) + (*_attributes[index_1].patch)(1,0)) / 2;
            centre2 = ((*_attributes[index_0].patch)(1,1) + (*_attributes[index_1].patch)(1,1)) / 2;
            centre3 = ((*_attributes[index_0].patch)(1,2) + (*_attributes[index_1].patch)(1,2)) / 2;
            centre4 = ((*_attributes[index_0].patch)(1,3) + (*_attributes[index_1].patch)(1,3)) / 2;

            (*_attributes[index_0].patch)(0,0) = centre1;
            (*_attributes[index_0].patch)(0,1) = centre2;
            (*_attributes[index_0].patch)(0,2) = centre3;
            (*_attributes[index_0].patch)(0,3) = centre4;

            (*_attributes[index_1].patch)(0,0) = centre1;
            (*_attributes[index_1].patch)(0,1) = centre2;
            (*_attributes[index_1].patch)(0,2) = centre3;
            (*_attributes[index_1].patch)(0,3) = centre4;

            _attributes[index_0].neighbours[N] = &_attributes[index_1];
            _attributes[index_0].connection_type[N] = N;
            _attributes[index_1].neighbours[N] = &_attributes[index_0];
            _attributes[index_1].connection_type[N] = N;


            return GenerateImageOfSelectedPatches(index_0, index_1);
        }

        if (direction_1 == S)
        {
            centre1 = ((*_attributes[index_0].patch)(1,0) + (*_attributes[index_1].patch)(2,0)) / 2;
            centre2 = ((*_attributes[index_0].patch)(1,1) + (*_attributes[index_1].patch)(2,1)) / 2;
            centre3 = ((*_attributes[index_0].patch)(1,2) + (*_attributes[index_1].patch)(2,2)) / 2;
            centre4 = ((*_attributes[index_0].patch)(1,3) + (*_attributes[index_1].patch)(2,3)) / 2;

            (*_attributes[index_0].patch)(0,0) = centre1;
            (*_attributes[index_0].patch)(0,1) = centre2;
            (*_attributes[index_0].patch)(0,2) = centre3;
            (*_attributes[index_0].patch)(0,3) = centre4;

            (*_attributes[index_1].patch)(3,0) = centre1;
            (*_attributes[index_1].patch)(3,1) = centre2;
            (*_attributes[index_1].patch)(3,2) = centre3;
            (*_attributes[index_1].patch)(3,3) = centre4;

            _attributes[index_0].neighbours[N] = &_attributes[index_1];
            _attributes[index_0].connection_type[N] = S;
            _attributes[index_1].neighbours[S] = &_attributes[index_0];
            _attributes[index_1].connection_type[S] = N;


            return GenerateImageOfSelectedPatches(index_0, index_1);
        }


    }

    if (direction_0 == SE)
    {
        if (direction_1 == NW)
        {
            centre1 = ((*_attributes[index_0].patch)(3,2) + (*_attributes[index_1].patch)(0,1)) / 2;
            centre2 = ((*_attributes[index_0].patch)(2,3) + (*_attributes[index_1].patch)(1,0)) / 2;

            centre3 = (centre1 + centre2) / 2;

            (*_attributes[index_0].patch)(2,3) = (*_attributes[index_0].patch)(2,3) + centre1 - centre3;
            (*_attributes[index_0].patch)(3,2) = (*_attributes[index_0].patch)(3,2) + centre2 - centre3;
            (*_attributes[index_0].patch)(3,3) = centre3;

            (*_attributes[index_1].patch)(0,0) = centre3;
            (*_attributes[index_1].patch)(0,1) = (*_attributes[index_1].patch)(0,1) + centre2 - centre3;
            (*_attributes[index_1].patch)(1,0) = (*_attributes[index_1].patch)(1,0) + centre1 - centre3;

            _attributes[index_0].neighbours[SE] = &_attributes[index_1];
            _attributes[index_0].connection_type[SE] = NW;
            _attributes[index_1].neighbours[NW] = &_attributes[index_0];
            _attributes[index_1].connection_type[NW] = SE;


            return GenerateImageOfSelectedPatches(index_0, index_1);
        }

        if (direction_1 == SE)
        {
            centre1 = ((*_attributes[index_0].patch)(3,2) + (*_attributes[index_1].patch)(3,2)) / 2;
            centre2 = ((*_attributes[index_0].patch)(2,3) + (*_attributes[index_1].patch)(2,3)) / 2;

            centre3 = (centre1 + centre2) / 2;

            (*_attributes[index_0].patch)(2,3) = (*_attributes[index_0].patch)(2,3) + centre1 - centre3;
            (*_attributes[index_0].patch)(3,2) = (*_attributes[index_0].patch)(3,2) + centre2 - centre3;
            (*_attributes[index_0].patch)(3,3) = centre3;

            (*_attributes[index_1].patch)(3,3) = centre3;
            (*_attributes[index_1].patch)(3,2) = (*_attributes[index_1].patch)(3,2) + centre2 - centre3;
            (*_attributes[index_1].patch)(2,3) = (*_attributes[index_1].patch)(2,3) + centre1 - centre3;

            _attributes[index_0].neighbours[SE] = &_attributes[index_1];
            _attributes[index_0].connection_type[SE] = SE;
            _attributes[index_1].neighbours[SE] = &_attributes[index_0];
            _attributes[index_1].connection_type[SE] = SE;


            return GenerateImageOfSelectedPatches(index_0, index_1);
        }

        if (direction_1 == SW)
        {
            centre1 = ((*_attributes[index_0].patch)(3,2) + (*_attributes[index_1].patch)(3,1)) / 2;
            centre2 = ((*_attributes[index_0].patch)(2,3) + (*_attributes[index_1].patch)(2,0)) / 2;

            centre3 = (centre1 + centre2) / 2;

            (*_attributes[index_0].patch)(2,3) = (*_attributes[index_0].patch)(2,3) + centre1 - centre3;
            (*_attributes[index_0].patch)(3,2) = (*_attributes[index_0].patch)(3,2) + centre2 - centre3;
            (*_attributes[index_0].patch)(3,3) = centre3;

            (*_attributes[index_1].patch)(3,0) = centre3;
            (*_attributes[index_1].patch)(3,1) = (*_attributes[index_1].patch)(3,1) + centre2 - centre3;
            (*_attributes[index_1].patch)(2,0) = (*_attributes[index_1].patch)(2,0) + centre1 - centre3;

            _attributes[index_0].neighbours[SE] = &_attributes[index_1];
            _attributes[index_0].connection_type[SE] = SW;
            _attributes[index_1].neighbours[SW] = &_attributes[index_0];
            _attributes[index_1].connection_type[SW] = SE;

            return GenerateImageOfSelectedPatches(index_0, index_1);

        }

        if (direction_1 == NE)
        {
            centre1 = ((*_attributes[index_0].patch)(3,2) + (*_attributes[index_1].patch)(0,2)) / 2;
            centre2 = ((*_attributes[index_0].patch)(2,3) + (*_attributes[index_1].patch)(1,3)) / 2;

            centre3 = (centre1 + centre2) / 2;

            (*_attributes[index_0].patch)(2,3) = (*_attributes[index_0].patch)(2,3) + centre1 - centre3;
            (*_attributes[index_0].patch)(3,2) = (*_attributes[index_0].patch)(3,2) + centre2 - centre3;
            (*_attributes[index_0].patch)(3,3) = centre3;

            (*_attributes[index_1].patch)(0,3) = centre3;
            (*_attributes[index_1].patch)(0,2) = (*_attributes[index_1].patch)(0,2) + centre2 - centre3;
            (*_attributes[index_1].patch)(1,3) = (*_attributes[index_1].patch)(1,3) + centre1 - centre3;

            _attributes[index_0].neighbours[SE] = &_attributes[index_1];
            _attributes[index_0].connection_type[SE] = NE;
            _attributes[index_1].neighbours[NE] = &_attributes[index_0];
            _attributes[index_1].connection_type[NE] = SE;

            return GenerateImageOfSelectedPatches(index_0, index_1);
        }
    }

    if (direction_0 == NW)
    {
        if (direction_1 == SE)
        {
            centre1 = ((*_attributes[index_0].patch)(0,1) + (*_attributes[index_1].patch)(3,2)) / 2;
            centre2 = ((*_attributes[index_0].patch)(1,0) + (*_attributes[index_1].patch)(2,3)) / 2;

            centre3 = (centre1 + centre2) / 2;

            (*_attributes[index_0].patch)(0,1) = (*_attributes[index_0].patch)(0,1) + centre2 - centre3;
            (*_attributes[index_0].patch)(1,0) = (*_attributes[index_0].patch)(1,0) + centre1 - centre3;
            (*_attributes[index_0].patch)(0,0) = centre3;

            (*_attributes[index_1].patch)(3,3) = centre3;
            (*_attributes[index_1].patch)(3,2) = (*_attributes[index_1].patch)(3,2) + centre2 - centre3;
            (*_attributes[index_1].patch)(2,3) = (*_attributes[index_1].patch)(2,3) + centre1 - centre3;

            _attributes[index_0].neighbours[NW] = &_attributes[index_1];
            _attributes[index_0].connection_type[NW] = SE;
            _attributes[index_1].neighbours[SE] = &_attributes[index_0];
            _attributes[index_1].connection_type[SE] = NW;

            return GenerateImageOfSelectedPatches(index_0, index_1);

        }

        if (direction_1 == NW)
        {
            centre1 = ((*_attributes[index_0].patch)(0,1) + (*_attributes[index_1].patch)(0,1)) / 2;
            centre2 = ((*_attributes[index_0].patch)(1,0) + (*_attributes[index_1].patch)(1,0)) / 2;

            centre3 = (centre1 + centre2) / 2;

            (*_attributes[index_0].patch)(0,1) = (*_attributes[index_0].patch)(0,1) + centre2 - centre3;
            (*_attributes[index_0].patch)(1,0) = (*_attributes[index_0].patch)(1,0) + centre1 - centre3;
            (*_attributes[index_0].patch)(0,0) = centre3;

            (*_attributes[index_1].patch)(0,0) = centre3;
            (*_attributes[index_1].patch)(1,0) = (*_attributes[index_1].patch)(1,0) + centre1 - centre3;
            (*_attributes[index_1].patch)(0,1) = (*_attributes[index_1].patch)(0,1) + centre2 - centre3;

            _attributes[index_0].neighbours[NW] = &_attributes[index_1];
            _attributes[index_0].connection_type[NW] = NW;
            _attributes[index_1].neighbours[NW] = &_attributes[index_0];
            _attributes[index_1].connection_type[NW] = NW;

            return GenerateImageOfSelectedPatches(index_0, index_1);

        }

        if (direction_1 == SW)
        {
            centre1 = ((*_attributes[index_0].patch)(0,1) + (*_attributes[index_1].patch)(3,1)) / 2;
            centre2 = ((*_attributes[index_0].patch)(1,0) + (*_attributes[index_1].patch)(2,0)) / 2;

            centre3 = (centre1 + centre2) / 2;

            (*_attributes[index_0].patch)(0,1) = (*_attributes[index_0].patch)(0,1) + centre2 - centre3;
            (*_attributes[index_0].patch)(1,0) = (*_attributes[index_0].patch)(1,0) + centre1 - centre3;
            (*_attributes[index_0].patch)(0,0) = centre3;

            (*_attributes[index_1].patch)(3,0) = centre3;
            (*_attributes[index_1].patch)(2,0) = (*_attributes[index_1].patch)(2,0) + centre1 - centre3;
            (*_attributes[index_1].patch)(3,1) = (*_attributes[index_1].patch)(3,1) + centre2 - centre3;

            _attributes[index_0].neighbours[NW] = &_attributes[index_1];
            _attributes[index_0].connection_type[NW] = SW;
            _attributes[index_1].neighbours[SW] = &_attributes[index_0];
            _attributes[index_1].connection_type[SW] = NW;

            return GenerateImageOfSelectedPatches(index_0, index_1);


        }

        if (direction_1 == NE)
        {
            centre1 = ((*_attributes[index_0].patch)(0,1) + (*_attributes[index_1].patch)(0,2)) / 2;
            centre2 = ((*_attributes[index_0].patch)(1,0) + (*_attributes[index_1].patch)(1,3)) / 2;

            centre3 = (centre1 + centre2) / 2;

            (*_attributes[index_0].patch)(0,1) = (*_attributes[index_0].patch)(0,1) + centre2 - centre3;
            (*_attributes[index_0].patch)(1,0) = (*_attributes[index_0].patch)(1,0) + centre1 - centre3;
            (*_attributes[index_0].patch)(0,0) = centre3;

            (*_attributes[index_1].patch)(0,3) = centre3;
            (*_attributes[index_1].patch)(1,3) = (*_attributes[index_1].patch)(1,3) + centre1 - centre3;
            (*_attributes[index_1].patch)(0,2) = (*_attributes[index_1].patch)(0,2) + centre2 - centre3;

            _attributes[index_0].neighbours[NW] = &_attributes[index_1];
            _attributes[index_0].connection_type[NW] = NE;
            _attributes[index_1].neighbours[NE] = &_attributes[index_0];
            _attributes[index_1].connection_type[NE] = NW;

            return GenerateImageOfSelectedPatches(index_0, index_1);

        }
    }

    if (direction_0 == SW)
    {

        if (direction_1 == SE)
        {
            centre1 = ((*_attributes[index_0].patch)(3,1) + (*_attributes[index_1].patch)(3,2)) / 2;
            centre2 = ((*_attributes[index_0].patch)(2,0) + (*_attributes[index_1].patch)(2,3)) / 2;

            centre3 = (centre1 + centre2) / 2;

            (*_attributes[index_0].patch)(3,1) = (*_attributes[index_0].patch)(3,1) + centre2 - centre3;
            (*_attributes[index_0].patch)(2,0) = (*_attributes[index_0].patch)(2,0) + centre1 - centre3;
            (*_attributes[index_0].patch)(3,0) = centre3;

            (*_attributes[index_1].patch)(3,3) = centre3;
            (*_attributes[index_1].patch)(2,3) = (*_attributes[index_1].patch)(2,3) + centre1 - centre3;
            (*_attributes[index_1].patch)(3,2) = (*_attributes[index_1].patch)(3,2) + centre2 - centre3;

            _attributes[index_0].neighbours[SW] = &_attributes[index_1];
            _attributes[index_0].connection_type[SW] = SE;
            _attributes[index_1].neighbours[SE] = &_attributes[index_0];
            _attributes[index_1].connection_type[SE] = SW;

            return GenerateImageOfSelectedPatches(index_0, index_1);

        }

        if (direction_1 == NW)
        {
            centre1 = ((*_attributes[index_0].patch)(3,1) + (*_attributes[index_1].patch)(0,1)) / 2;
            centre2 = ((*_attributes[index_0].patch)(2,0) + (*_attributes[index_1].patch)(1,0)) / 2;

            centre3 = (centre1 + centre2) / 2;

            (*_attributes[index_0].patch)(3,1) = (*_attributes[index_0].patch)(3,1) + centre2 - centre3;
            (*_attributes[index_0].patch)(2,0) = (*_attributes[index_0].patch)(2,0) + centre1 - centre3;
            (*_attributes[index_0].patch)(3,0) = centre3;

            (*_attributes[index_1].patch)(0,0) = centre3;
            (*_attributes[index_1].patch)(1,0) = (*_attributes[index_1].patch)(1,0) + centre1 - centre3;
            (*_attributes[index_1].patch)(0,1) = (*_attributes[index_1].patch)(0,1) + centre2 - centre3;

            _attributes[index_0].neighbours[SW] = &_attributes[index_1];
            _attributes[index_0].connection_type[SW] = NW;
            _attributes[index_1].neighbours[NW] = &_attributes[index_0];
            _attributes[index_1].connection_type[NW] = SW;

            return GenerateImageOfSelectedPatches(index_0, index_1);

        }

        if (direction_1 == SW)
        {
            centre1 = ((*_attributes[index_0].patch)(3,1) + (*_attributes[index_1].patch)(3,1)) / 2;
            centre2 = ((*_attributes[index_0].patch)(2,0) + (*_attributes[index_1].patch)(2,0)) / 2;

            centre3 = (centre1 + centre2) / 2;

            (*_attributes[index_0].patch)(3,1) = (*_attributes[index_0].patch)(3,1) + centre2 - centre3;
            (*_attributes[index_0].patch)(2,0) = (*_attributes[index_0].patch)(2,0) + centre1 - centre3;
            (*_attributes[index_0].patch)(3,0) = centre3;

            (*_attributes[index_1].patch)(3,0) = centre3;
            (*_attributes[index_1].patch)(2,0) = (*_attributes[index_1].patch)(2,0) + centre1 - centre3;
            (*_attributes[index_1].patch)(3,1) = (*_attributes[index_1].patch)(3,1) + centre2 - centre3;

            _attributes[index_0].neighbours[SW] = &_attributes[index_1];
            _attributes[index_0].connection_type[SW] = SW;
            _attributes[index_1].neighbours[SW] = &_attributes[index_0];
            _attributes[index_1].connection_type[SW] = SW;

            return GenerateImageOfSelectedPatches(index_0, index_1);

        }

        if (direction_1 == NE)
        {
            centre1 = ((*_attributes[index_0].patch)(3,1) + (*_attributes[index_1].patch)(0,2)) / 2;
            centre2 = ((*_attributes[index_0].patch)(2,0) + (*_attributes[index_1].patch)(1,3)) / 2;

            centre3 = (centre1 + centre2) / 2;

            (*_attributes[index_0].patch)(3,1) = (*_attributes[index_0].patch)(3,1) + centre2 - centre3;
            (*_attributes[index_0].patch)(2,0) = (*_attributes[index_0].patch)(2,0) + centre1 - centre3;
            (*_attributes[index_0].patch)(3,0) = centre3;

            (*_attributes[index_1].patch)(0,3) = centre3;
            (*_attributes[index_1].patch)(1,3) = (*_attributes[index_1].patch)(1,3) + centre1 - centre3;
            (*_attributes[index_1].patch)(0,2) = (*_attributes[index_1].patch)(0,2) + centre2 - centre3;

            _attributes[index_0].neighbours[SW] = &_attributes[index_1];
            _attributes[index_0].connection_type[SW] = NE;
            _attributes[index_1].neighbours[NE] = &_attributes[index_0];
            _attributes[index_1].connection_type[NE] = SW;

            return GenerateImageOfSelectedPatches(index_0, index_1);

        }
    }

    if (direction_0 == NE)
    {
        if (direction_1 == SE)
        {
            centre1 = ((*_attributes[index_0].patch)(0,2) + (*_attributes[index_1].patch)(3,2)) / 2;
            centre2 = ((*_attributes[index_0].patch)(1,3) + (*_attributes[index_1].patch)(2,3)) / 2;

            centre3 = (centre1 + centre2) / 2;

            (*_attributes[index_0].patch)(0,2) = (*_attributes[index_0].patch)(0,2) + centre2 - centre3;
            (*_attributes[index_0].patch)(1,3) = (*_attributes[index_0].patch)(1,3) + centre1 - centre3;
            (*_attributes[index_0].patch)(0,3) = centre3;

            (*_attributes[index_1].patch)(3,3) = centre3;
            (*_attributes[index_1].patch)(2,3) = (*_attributes[index_1].patch)(2,3) + centre1 - centre3;
            (*_attributes[index_1].patch)(3,2) = (*_attributes[index_1].patch)(3,2) + centre2 - centre3;

            _attributes[index_0].neighbours[NE] = &_attributes[index_1];
            _attributes[index_0].connection_type[NE] = SE;
            _attributes[index_1].neighbours[SE] = &_attributes[index_0];
            _attributes[index_1].connection_type[SE] = NE;

            return GenerateImageOfSelectedPatches(index_0, index_1);

        }

        if (direction_1 == NW)
        {
            centre1 = ((*_attributes[index_0].patch)(0,2) + (*_attributes[index_1].patch)(0,1)) / 2;
            centre2 = ((*_attributes[index_0].patch)(1,3) + (*_attributes[index_1].patch)(1,0)) / 2;

            centre3 = (centre1 + centre2) / 2;

            (*_attributes[index_0].patch)(0,2) = (*_attributes[index_0].patch)(0,2) + centre2 - centre3;
            (*_attributes[index_0].patch)(1,3) = (*_attributes[index_0].patch)(1,3) + centre1 - centre3;
            (*_attributes[index_0].patch)(0,3) = centre3;

            (*_attributes[index_1].patch)(0,0) = centre3;
            (*_attributes[index_1].patch)(1,0) = (*_attributes[index_1].patch)(1,0) + centre1 - centre3;
            (*_attributes[index_1].patch)(0,1) = (*_attributes[index_1].patch)(0,1) + centre2 - centre3;

            _attributes[index_0].neighbours[NE] = &_attributes[index_1];
            _attributes[index_0].connection_type[NE] = NW;
            _attributes[index_1].neighbours[NW] = &_attributes[index_0];
            _attributes[index_1].connection_type[NW] = NE;

            return GenerateImageOfSelectedPatches(index_0, index_1);


        }

        if (direction_1 == SW)
        {
            centre1 = ((*_attributes[index_0].patch)(0,2) + (*_attributes[index_1].patch)(3,1)) / 2;
            centre2 = ((*_attributes[index_0].patch)(1,3) + (*_attributes[index_1].patch)(2,0)) / 2;

            centre3 = (centre1 + centre2) / 2;

            (*_attributes[index_0].patch)(0,2) = (*_attributes[index_0].patch)(0,2) + centre2 - centre3;
            (*_attributes[index_0].patch)(1,3) = (*_attributes[index_0].patch)(1,3) + centre1 - centre3;
            (*_attributes[index_0].patch)(0,3) = centre3;

            (*_attributes[index_1].patch)(3,0) = centre3;
            (*_attributes[index_1].patch)(2,0) = (*_attributes[index_1].patch)(2,0) + centre1 - centre3;
            (*_attributes[index_1].patch)(3,1) = (*_attributes[index_1].patch)(3,1) + centre2 - centre3;

            _attributes[index_0].neighbours[NE] = &_attributes[index_1];
            _attributes[index_0].connection_type[NE] = SW;
            _attributes[index_1].neighbours[SW] = &_attributes[index_0];
            _attributes[index_1].connection_type[SW] = NE;

            return GenerateImageOfSelectedPatches(index_0, index_1);

        }

        if (direction_1 == NE)
        {
            centre1 = ((*_attributes[index_0].patch)(0,2) + (*_attributes[index_1].patch)(0,2)) / 2;
            centre2 = ((*_attributes[index_0].patch)(1,3) + (*_attributes[index_1].patch)(1,3)) / 2;

            centre3 = (centre1 + centre2) / 2;

            (*_attributes[index_0].patch)(0,2) = (*_attributes[index_0].patch)(0,2) + centre2 - centre3;
            (*_attributes[index_0].patch)(1,3) = (*_attributes[index_0].patch)(1,3) + centre1 - centre3;
            (*_attributes[index_0].patch)(0,3) = centre3;

            (*_attributes[index_1].patch)(0,3) = centre3;
            (*_attributes[index_1].patch)(1,3) = (*_attributes[index_1].patch)(1,3) + centre1 - centre3;
            (*_attributes[index_1].patch)(0,2) = (*_attributes[index_1].patch)(0,2) + centre2 - centre3;

            _attributes[index_0].neighbours[NE] = &_attributes[index_1];
            _attributes[index_0].connection_type[NE] = NE;
            _attributes[index_1].neighbours[NE] = &_attributes[index_0];
            _attributes[index_1].connection_type[NE] = NE;

            return GenerateImageOfSelectedPatches(index_0, index_1);

        }
    }

  return GL_FALSE;
}


GLboolean FirstOrderAlgebraicTrigonometricCompositePatch3::GenerateImageOfSelectedPatches(GLuint index_0, GLuint index_1)
{
    if (_attributes[index_0].patch == nullptr || _attributes[index_1].patch == nullptr)
    {
        return GL_FALSE;
    }

    delete _attributes[index_0].image; _attributes[index_0].image = nullptr;

    _attributes[index_0].image = _attributes[index_0].patch->GenerateImage(_uDivPointCount, _vDivPointCount);

    if (!_attributes[index_0].image)
    {
        return GL_FALSE;
    }

    if (!_attributes[index_0].image->UpdateVertexBufferObjects())
    {
        return GL_FALSE;
    }

    _attributes[index_0]._uIsoparametricLines = _attributes[index_0].patch->GenerateUIsoparametricLines(_uIsoLineCount, 2, _uDivPointCount);
    _attributes[index_0]._vIsoparametricLines = _attributes[index_0].patch->GenerateVIsoparametricLines(_vIsoLineCount, 2, _vDivPointCount);

    for (GLuint i = 0; i < _attributes[index_0]._uIsoparametricLines->GetColumnCount(); i++)
    {
        (*_attributes[index_0]._uIsoparametricLines)[i]->UpdateVertexBufferObjects();
    }

    for (GLuint i = 0; i < _attributes[index_0]._vIsoparametricLines->GetColumnCount(); i++)
    {
        (*_attributes[index_0]._vIsoparametricLines)[i]->UpdateVertexBufferObjects();
    }

    if (!_attributes[index_0].patch->UpdateVertexBufferObjectsOfData())
    {
        return GL_FALSE;
    }

    delete _attributes[index_1].image; _attributes[index_1].image = nullptr;

    _attributes[index_1].image = _attributes[index_1].patch->GenerateImage(_uDivPointCount, _vDivPointCount);

    if (!_attributes[index_1].image)
    {
        return GL_FALSE;
    }

    if (!_attributes[index_1].image->UpdateVertexBufferObjects())
    {
        return GL_FALSE;
    }

    _attributes[index_1]._uIsoparametricLines = _attributes[index_1].patch->GenerateUIsoparametricLines(_uIsoLineCount, 2, _uDivPointCount);
    _attributes[index_1]._vIsoparametricLines = _attributes[index_1].patch->GenerateVIsoparametricLines(_vIsoLineCount, 2, _vDivPointCount);

    for (GLuint i = 0; i < _attributes[index_1]._uIsoparametricLines->GetColumnCount(); i++)
    {
        (*_attributes[index_1]._uIsoparametricLines)[i]->UpdateVertexBufferObjects();
    }

    for (GLuint i = 0; i < _attributes[index_1]._vIsoparametricLines->GetColumnCount(); i++)
    {
        (*_attributes[index_1]._vIsoparametricLines)[i]->UpdateVertexBufferObjects();
    }

    if (!_attributes[index_1].patch->UpdateVertexBufferObjectsOfData())
    {
        return GL_FALSE;
    }

    return GL_TRUE;
}


GLboolean FirstOrderAlgebraicTrigonometricCompositePatch3::InsertNewPatch2(DCoordinate3 *coord, Material *material, GLenum usage_flag)
{
    cout << _patch_count << endl;
    GLuint index = _patch_count;
    _attributes[index].patch = new (nothrow) FirstOrderAlgebraicTrigonometricPatch3();
    _attributes[index].material = material;

    if(!_attributes[index].patch)
    {
        DeleteExistingPatch(index);
        return GL_FALSE;
    }

    _attributes[index].patch->SetData(0, 0, coord[0]);
    _attributes[index].patch->SetData(0, 1, coord[1]);
    _attributes[index].patch->SetData(0, 2, coord[2]);
    _attributes[index].patch->SetData(0, 3, coord[3]);

    _attributes[index].patch->SetData(1, 0, coord[4]);
    _attributes[index].patch->SetData(1, 1, coord[5]);
    _attributes[index].patch->SetData(1, 2, coord[6]);
    _attributes[index].patch->SetData(1, 3, coord[7]);

    _attributes[index].patch->SetData(2, 0, coord[8]);
    _attributes[index].patch->SetData(2, 1, coord[9]);
    _attributes[index].patch->SetData(2, 2, coord[10]);
    _attributes[index].patch->SetData(2, 3, coord[11]);

    _attributes[index].patch->SetData(3, 0, coord[12]);
    _attributes[index].patch->SetData(3, 1, coord[13]);
    _attributes[index].patch->SetData(3, 2, coord[14]);
    _attributes[index].patch->SetData(3, 3, coord[15]);



    _attributes[index].patch->UpdateVertexBufferObjectsOfData();
    _attributes[index].image = _attributes[index].patch->GenerateImage(30, 30, usage_flag);

    if (_attributes[index].image)
        _attributes[index].image->UpdateVertexBufferObjects();

    _attributes[index]._uIsoparametricLines = _attributes[index].patch->GenerateUIsoparametricLines(_uIsoLineCount, 2, _uDivPointCount);
    _attributes[index]._vIsoparametricLines = _attributes[index].patch->GenerateVIsoparametricLines(_vIsoLineCount, 2, _vDivPointCount);

    for (GLuint i = 0; i < _attributes[index]._uIsoparametricLines->GetColumnCount(); i++)
    {
        (*_attributes[index]._uIsoparametricLines)[i]->UpdateVertexBufferObjects();
    }

    for (GLuint i = 0; i < _attributes[index]._vIsoparametricLines->GetColumnCount(); i++)
    {
        (*_attributes[index]._vIsoparametricLines)[i]->UpdateVertexBufferObjects();
    }

    _patch_count++;

    return GL_TRUE;

}

GLboolean FirstOrderAlgebraicTrigonometricCompositePatch3::ContinueExistingPatch(GLuint index, Direction direction, Material *material)
{
    DCoordinate3 *coord = new DCoordinate3[16];

    if (_attributes[index].neighbours[direction] != nullptr)
    {
        cout << "Nem kothetek ossze osszekotottet!\n";
        return false;
    }

    if (direction == N)
    {
        for (GLuint i = 0; i < 4; i ++)
        {
            coord[i + 4 * 0] = (*_attributes[index].patch)(0, 3) + i * ((*_attributes[index].patch)(0, 3) - (*_attributes[index].patch)(0, 2));
            coord[i + 4 * 1] = (*_attributes[index].patch)(1, 3) + i * ((*_attributes[index].patch)(1, 3) - (*_attributes[index].patch)(1, 2));
            coord[i + 4 * 2] = (*_attributes[index].patch)(2, 3) + i * ((*_attributes[index].patch)(2, 3) - (*_attributes[index].patch)(2, 2));
            coord[i + 4 * 3] = (*_attributes[index].patch)(3, 3) + i * ((*_attributes[index].patch)(3, 3) - (*_attributes[index].patch)(3, 2));
        }
    }

    if (direction == W)
    {
        for (GLuint i = 0; i < 4; i ++)
        {
            coord[16 - i * 4 - 4] = (*_attributes[index].patch)(0, 0) + i * ((*_attributes[index].patch)(0, 0) - (*_attributes[index].patch)(1, 0));
            coord[16 - i * 4 - 3] = (*_attributes[index].patch)(0, 1) + i * ((*_attributes[index].patch)(0, 1) - (*_attributes[index].patch)(1, 1));
            coord[16 - i * 4 - 2] = (*_attributes[index].patch)(0, 2) + i * ((*_attributes[index].patch)(0, 2) - (*_attributes[index].patch)(1, 2));
            coord[16 - i * 4 - 1] = (*_attributes[index].patch)(0, 3) + i * ((*_attributes[index].patch)(0, 3) - (*_attributes[index].patch)(1, 3));
        }
    }

    if (direction == S)
    {
        for (GLuint i = 0; i < 4; i ++)
        {
            coord[0 * 4 + 3 - i] = (*_attributes[index].patch)(0, 0) + i * ((*_attributes[index].patch)(0, 0) - (*_attributes[index].patch)(0, 1));
            coord[1 * 4 + 3 - i] = (*_attributes[index].patch)(1, 0) + i * ((*_attributes[index].patch)(1, 0) - (*_attributes[index].patch)(1, 1));
            coord[2 * 4 + 3 - i] = (*_attributes[index].patch)(2, 0) + i * ((*_attributes[index].patch)(2, 0) - (*_attributes[index].patch)(2, 1));
            coord[3 * 4 + 3 - i] = (*_attributes[index].patch)(3, 0) + i * ((*_attributes[index].patch)(3, 0) - (*_attributes[index].patch)(3, 1));
        }
    }

    if (direction == E)
    {
        for (GLuint i = 0; i < 4; i ++)
        {
            coord[i * 4 + 0] = (*_attributes[index].patch)(3, 0) + i * ((*_attributes[index].patch)(3, 0) - (*_attributes[index].patch)(2, 0));
            coord[i * 4 + 1] = (*_attributes[index].patch)(3, 1) + i * ((*_attributes[index].patch)(3, 1) - (*_attributes[index].patch)(2, 1));
            coord[i * 4 + 2] = (*_attributes[index].patch)(3, 2) + i * ((*_attributes[index].patch)(3, 2) - (*_attributes[index].patch)(2, 2));
            coord[i * 4 + 3] = (*_attributes[index].patch)(3, 3) + i * ((*_attributes[index].patch)(3, 3) - (*_attributes[index].patch)(2, 3));
        }
    }

    if (direction == NE)
    {
        DCoordinate3 q = (*_attributes[index].patch)(3, 3);
        DCoordinate3 u = (*_attributes[index].patch)(3, 3) - (*_attributes[index].patch)(3, 2);
        DCoordinate3 r = (*_attributes[index].patch)(3, 3) - (*_attributes[index].patch)(2, 3);

        for (GLuint i = 0; i < 4; i ++)
        {
            coord[i] = q + i * u;
        }

        for (GLuint i = 1; i < 4; i ++)
        {
            coord[i * 4] = q + i * r;
        }

        for (GLuint i = 1; i < 4; i ++)
        {
            for (GLuint j = 1; j < 4; j ++)
            {
                coord[j * 4 + i] += (q + j * r) + i * u;
                coord[i * 4 + j] += (q + i * r) + j * u;
            }
       }

        for (GLuint i = 1; i < 4; i ++)
        {
            for (GLuint j = 1; j < 4; j ++)
            {
                coord[j * 4 + i] /= 2.0;
            }
        }
    }

    if (direction == NW)
    {
        DCoordinate3 q = (*_attributes[index].patch)(0, 3);
        DCoordinate3 u = (*_attributes[index].patch)(0, 3) - (*_attributes[index].patch)(0, 2);
        DCoordinate3 l = (*_attributes[index].patch)(0, 3) - (*_attributes[index].patch)(1, 3);

        for (GLuint i = 0; i < 4; i ++)
        {
            coord[12 + i] = q + i * u;
        }

        for (GLuint i = 1; i < 4; i ++)
        {
            coord[12 - i * 4] = q + i * l;
        }

        for (GLuint i = 1; i < 4; i ++)
        {
            for (GLuint j = 1; j < 4; j ++)
            {
                coord[(3 - j) * 4 + i] += (q + j * l) + i * u;
                coord[(3 - i) * 4 + j] += (q + i * l) + j * u;
            }
       }

        for (GLuint i = 1; i < 4; i ++)
        {
            for (GLuint j = 0; j < 3; j ++)
            {
                coord[j * 4 + i] /= 2.0;
            }
        }
    }

    if (direction == SW)
    {
        DCoordinate3 q = (*_attributes[index].patch)(0, 0);
        DCoordinate3 d = (*_attributes[index].patch)(0, 0) - (*_attributes[index].patch)(0, 1);
        DCoordinate3 l = (*_attributes[index].patch)(0, 0) - (*_attributes[index].patch)(1, 0);

        for (GLuint i = 0; i < 4; i ++)
        {
            coord[15 - i] = q + i * d;
        }

        for (GLuint i = 1; i < 4; i ++)
        {
            coord[15 - i * 4] = q + i * l;
        }

        for (GLuint i = 1; i < 4; i ++)
        {
            for (GLuint j = 1; j < 4; j ++)
            {
                coord[(3 - j) * 4 + (3 - i)] += (q + j * l) + i * d;
                coord[(3 - i) * 4 + (3 - j)] += (q + i * l) + j * d;
            }
        }

        for (GLuint i = 0; i < 3; i ++)
        {
            for (GLuint j = 0; j < 3; j ++)
            {
                coord[j * 4 + i] /= 2.0;
            }
        }
    }

    if (direction == SE)
    {
        DCoordinate3 q = (*_attributes[index].patch)(3, 0);
        DCoordinate3 d = (*_attributes[index].patch)(3, 0) - (*_attributes[index].patch)(3, 1);
        DCoordinate3 r = (*_attributes[index].patch)(3, 0) - (*_attributes[index].patch)(2, 0);

        for (GLuint i = 0; i < 4; i ++)
        {
            coord[3 - i] = q + i * d;
        }

        for (GLuint i = 1; i < 4; i ++)
        {
            coord[3 + i * 4] = q + i * r;
        }

        for (GLuint i = 1; i < 4; i ++)
        {
            for (GLuint j = 1; j < 4; j ++)
            {
                coord[j * 4 + (3 - i)] += (q + j * r) + i * d;
                coord[i * 4 + (3 - j)] += (q + i * r) + j * d;
            }
        }

        for (GLuint i = 0; i < 3; i ++)
        {
            for (GLuint j = 1; j < 4; j ++)
            {
                coord[j * 4 + i] /= 2.0;
            }
        }
    }

    InsertNewPatch2(coord, material);

    _attributes[index].neighbours[direction] = &_attributes[_patch_count - 1]; // uj
    //_attributes[index].neighbours_index[direction] = _patch_count - 1; // uj
    _attributes[index].connection_type[direction] = direction; // uj

    _attributes[_patch_count - 1].neighbours[(Direction)((direction + 4) % 8)] = &_attributes[index]; // uj
    //_attributes[_patch_count - 1].neighbours_index[(Direction)((direction + 4) % 8)] = index; // uj
    _attributes[_patch_count - 1].connection_type[(Direction)((direction + 4) % 8)] = (Direction)((direction + 4) % 8); // uj

    return true;
}

void FirstOrderAlgebraicTrigonometricCompositePatch3::TranslateSelectedPatch(int index, int coord, double value) // atirtam
{
    for (GLuint i = 0; i < 4; i ++)
    {
        for (GLuint j = 0; j < 4; j ++)
        {
            (*_attributes[index].patch)(i, j)[coord] += value;
        }
    }
}

void FirstOrderAlgebraicTrigonometricCompositePatch3::set_selected_patch_index(GLuint index)
{
    _selected_patch_index = index;
}

void FirstOrderAlgebraicTrigonometricCompositePatch3::SetTextureOfSelectedPatch(GLuint index, QOpenGLTexture *texture)
{
    _attributes[index].texture = texture;
}

void FirstOrderAlgebraicTrigonometricCompositePatch3::SetMaterialOfSelectedPatch(GLuint index, Material *material)
{
    _attributes[index].material = material;
}

void FirstOrderAlgebraicTrigonometricCompositePatch3::SetShowTexture(bool value)
{
    _show_texture = value;
}

void FirstOrderAlgebraicTrigonometricCompositePatch3::load()
{
    DCoordinate3 *data = new DCoordinate3[16];
    GLuint patch_count;
    ifstream in("result.txt");
    in >> patch_count;
    for (GLuint k = 0; k < patch_count; k ++)
    {
        for (GLuint i = 0; i < 4; i ++)
        {
            for (GLuint j = 0; j < 4; j ++)
            {
                in >> data[i * 4 + j];
            }
        }

        InsertNewPatch2(data, &MatFBEmerald);

        // szomik

//        GLuint index;
//        GLuint type;
//        Direction dir;
//        for (GLuint i = 0; i < 8; i ++)
//        {
//            in >> index >> type;
//            cout << index << endl;
//            dir = (Direction) type;
//            if (index < 1000)
//            {
//                _attributes[_patch_count - 1].neighbours[i] = &_attributes[index];
//                _attributes[_patch_count - 1].neighbours_index[i] = index;
//                _attributes[_patch_count - 1].connection_type[i] = dir;
//            }
//        }
    }
}

void FirstOrderAlgebraicTrigonometricCompositePatch3::save()
{
    ofstream out("result.txt");
    out << _patch_count << endl;
    for (GLuint k = 0; k < _patch_count; k ++)
    {
        for (GLuint i = 0; i < 4; i ++)
        {
            for (GLuint j = 0; j < 4; j ++)
            {
                out << (*_attributes[k].patch)(i, j) << " ";
            }
            out << endl;
        }

        // szomik

//        for (GLuint i = 0; i < 8; i ++)
//        {
//            out << _attributes[k].neighbours_index[i] << endl;
//            out << _attributes[k].connection_type[i] << endl;
//        }
        out << endl;
    }
}
