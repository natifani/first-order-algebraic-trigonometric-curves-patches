#include "GLWidget.h"

#if !defined(__APPLE__)
#include <GL/glu.h>
#endif

#include <iostream>
using namespace std;

#include <fstream>

#include <Core/Exceptions.h>
#include <Core/DCoordinates3.h>
#include <Core/Matrices.h>
#include <Core/RealSquareMatrices.h>
#include <Core/Materials.h>
#include <Core/Constants.h>
#include <Core/Colors4.h>
#include <random>
#include <iomanip>

#include <QOpenGLTexture>

namespace cagd
{
    //--------------------------------
    // special and default constructor
    //--------------------------------
    GLWidget::GLWidget(QWidget *parent, MergeWindow* merge_window, ContinueWindow* continue_window, JoinWindow* join_window, JoinPatchWindow *join_patch_window, MergePatchWindow *merge_patch_window, ContinuePatchWindow *continue_patch_window): QOpenGLWidget(parent)
    {
        // foat
        _merge_window = merge_window;
        _continue_window = continue_window;
        _join_window = join_window;
        _join_patch_window = join_patch_window;
        _merge_patch_window = merge_patch_window;
        _continue_patch_window = continue_patch_window;
    }

    //--------------------------------------------------------------------------------------
    // this virtual function is called once before the first call to paintGL() or resizeGL()
    //--------------------------------------------------------------------------------------
    void GLWidget::initializeGL()
    {
        // creating a perspective projection matrix
        glMatrixMode(GL_PROJECTION);

        glLoadIdentity();

        _aspect = static_cast<double>(width()) / static_cast<double>(height());
        _z_near = 1.0;
        _z_far  = 1000.0;
        _fovy   = 45.0;

        gluPerspective(_fovy, _aspect, _z_near, _z_far);

        // setting the model view matrix
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        _eye[0] = _eye[1] = 0.0; _eye[2] = 6.0;
        _center[0] = _center[1] = _center[2] = 0.0;
        _up[0] = _up[2] = 0.0; _up[1] = 1.0;

        gluLookAt(_eye[0], _eye[1], _eye[2], _center[0], _center[1], _center[2], _up[0], _up[1], _up[2]);

        // enabling the depth test
        glEnable(GL_DEPTH_TEST);

        // setting the background color
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

        // initial values of transformation parameters
        _angle_x = _angle_y = _angle_z = 0.0;
        _trans_x = _trans_y = _trans_z = 0.0;
        _zoom = 1.0;

        // ...

        try
        {
            // initializing the OpenGL Extension Wrangler library
            GLenum error = glewInit();

            if (error != GLEW_OK)
            {
                throw Exception("Could not initialize the OpenGL Extension Wrangler Library!");
            }

            if (!glewIsSupported("GL_VERSION_2_0"))
            {
                throw Exception("Your graphics card is not compatible with OpenGL 2.0+! "
                                "Try to update your driver or buy a new graphics adapter!");
            }

            // create and store your geometry in display lists or vertex buffer objects

            /*switch(_test)
            {
                case PATCH:
                    glEnable(GL_LIGHTING);
                    glEnable(GL_LIGHT0);
                    glEnable(GL_NORMALIZE);
                    _createPatch();
                    break;

                case ARC:
                    _createArc();
                    break;

                case SHADER:
                    glEnable(GL_LIGHTING);
                    glEnable(GL_LIGHT0);
                    glEnable(GL_NORMALIZE);
                    _createPatch2();
                    _initShader();
                    break;

                case FOAT_CURVES:
                    _createArc();
                    break;

                case FOAT_PATCHES:
                    glEnable(GL_LIGHTING);
                    glEnable(GL_LIGHT0);
                    glEnable(GL_NORMALIZE);
                    _createPatch2();

                break;
                default:
                    break;
            }*/
}
        catch (Exception &e)
        {
            cout << e << endl;
        }
    }

    //-----------------------
    // the rendering function
    //-----------------------
    void GLWidget::paintGL()
    {
        // clears the color and depth buffers
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // stores/duplicates the original model view matrix
        glPushMatrix();

            // applying transformations
            glRotatef(_angle_x, 1.0, 0.0, 0.0);
            glRotatef(_angle_y, 0.0, 1.0, 0.0);
            glRotatef(_angle_z, 0.0, 0.0, 1.0);
            glTranslated(_trans_x, _trans_y, _trans_z);
            glScaled(_zoom, _zoom, _zoom);

            // render your geometry (this is oldest OpenGL rendering technique, later we will use some advanced methods)

            switch(_test)
            {
                case PATCH:
                    glPushMatrix();
                    _renderPatch();
                    glPopMatrix();
                    break;

                case ARC:
                    _renderArc();
                    break;

                case SHADER:
                    _shader.Enable(GL_TRUE);
                        _renderPatch();
                    _shader.Disable();
                    break;

                case FOAT_CURVES:
                    glDisable(GL_LIGHTING);
                    glDisable(GL_LIGHT0);
                    glDisable(GL_NORMALIZE);
                    _renderArc();
                    break;

                case FOAT_PATCHES:
                        glEnable(GL_LIGHTING);
                        glEnable(GL_LIGHT0);
                        glEnable(GL_NORMALIZE);
                        if(!_show_texture)
                        _shader.Enable(GL_TRUE);
                            _renderPatch2();
                         if(!_show_texture)
                        _shader.Disable();
                            _renderPatchColors();
                        break;

                default:
                    _simpleTriangle();
            }

        // pops the current matrix stack, replacing the current matrix with the one below it on the stack,
        // i.e., the original model view matrix is restored
        glPopMatrix();
    }



    //----------------------------------------------------------------------------
    // when the main window is resized one needs to redefine the projection matrix
    //----------------------------------------------------------------------------
    void GLWidget::resizeGL(int w, int h)
    {
        // setting the new size of the rendering context
        glViewport(0, 0, w, h);

        // redefining the projection matrix
        glMatrixMode(GL_PROJECTION);

        glLoadIdentity();

        _aspect = static_cast<double>(w) / static_cast<double>(h);

        gluPerspective(_fovy, _aspect, _z_near, _z_far);

        // switching back to the model view matrix
        glMatrixMode(GL_MODELVIEW);

        update();
    }


    // 0. test
        void GLWidget::_simpleTriangle()
        {
            glColor3f(1.0f, 1.0f, 1.0f);
            glBegin(GL_LINES);
                glVertex3f(0.0f, 0.0f, 0.0f);
                glVertex3f(1.1f, 0.0f, 0.0f);

                glVertex3f(0.0f, 0.0f, 0.0f);
                glVertex3f(0.0f, 1.1f, 0.0f);

                glVertex3f(0.0f, 0.0f, 0.0f);
                glVertex3f(0.0f, 0.0f, 1.1f);
            glEnd();

            glBegin(GL_TRIANGLES);
                // attributes
                glColor3f(1.0f, 0.0f, 0.0f);
                // associated with position
                glVertex3f(1.0f, 0.0f, 0.0f);

                // attributes
                glColor3f(0.0, 1.0, 0.0);
                // associated with position
                glVertex3f(0.0, 1.0, 0.0);

                // attributes
                glColor3f(0.0f, 0.0f, 1.0f);
                // associated with position
                glVertex3f(0.0f, 0.0f, 1.0f);
            glEnd();
        }


        // 1. patch
        void GLWidget::_createPatch()
        {
            FOATPatches.InsertNewPatch(&MatFBBrass);
        }

        void GLWidget::_renderPatch()
        {
            FOATPatches.RenderAllPatches(0);
        }

        void GLWidget::_destroyPatch()
        {
            FOATPatches.DeleteAllPatches();
        }


        bool GLWidget::_createPatch2()
        {
            Material *m = new Material(MatFBRuby);
            return FOATPatches.InsertNewPatch(m);
        }

        bool GLWidget::_renderPatch2()
        {
            FOATPatches.RenderAllPatches(-1, GL_POINTS);
            FOATPatches.RenderAllPatches(0, GL_LINE_STRIP);

            if (_foat_visibility_of_first_order_derivatives)
                FOATPatches.RenderAllPatches(1, GL_LINES);

            if (_foat_visibility_of_second_order_derivatives)
                FOATPatches.RenderAllPatches(2, GL_LINES);

            return true;
        }

        void GLWidget::_destroyPatch2()
        {
            FOATPatches.DeleteAllPatches();
        }



        // 2. arc
        bool GLWidget::_createArc()
        {
            Color4 *c1 = new Color4(1,0,0);
            return FOATCurves.InsertNewArc(c1);
        }

        bool GLWidget::_renderArc()
        {
            FOATCurves.RenderAllArcs(-1, GL_POINTS);
            FOATCurves.RenderAllArcs(0, GL_LINE_STRIP);
            if (_foat_arc_show_tangents)
                FOATCurves.RenderAllArcs(1, GL_LINES);
            if (_foat_arc_show_acceleration_vectors)
                FOATCurves.RenderAllArcs(2, GL_LINES);

            return true;
        }

        void GLWidget::_destroyArc()
        {
            FOATCurves.DeleteAllArcs();
        }


        // 3. shader
        void GLWidget::_initShader(int index)
        {
            switch (index)
            {
                case 0:
                    _shader.InstallShaders("Shaders/directional_light.vert",
                                           "Shaders/directional_light.frag", GL_TRUE);
                    break;
                case 1:
                    _shader.InstallShaders("Shaders/two_sided_lighting.vert",
                                           "Shaders/two_sided_lighting.frag", GL_TRUE);
                    break;
                case 2:
                    _shader.InstallShaders("Shaders/toon.vert",
                                           "Shaders/toon.frag", GL_TRUE);
                    break;
                case 3:
                    _shader.InstallShaders("Shaders/reflection_lines.vert",
                                           "Shaders/reflection_lines.frag", GL_TRUE);
                    break;
                default:
                    break;
            }

            _shader.Enable();

            if (index == 3)
            {
                _shader.SetUniformVariable1f("scale_factor", _scale_factor);
                _shader.SetUniformVariable1f("smoothing", _smoothing);
                _shader.SetUniformVariable1f("shading", _shading);
            }
        }


    //-----------------------------------
    // implementation of the public slots
    //-----------------------------------

    void GLWidget::set_angle_x(int value)
    {
        if (_angle_x != value)
        {
            _angle_x = value;
            update();
        }
    }

    void GLWidget::set_angle_y(int value)
    {
        if (_angle_y != value)
        {
            _angle_y = value;
            update();
        }
    }

    void GLWidget::set_angle_z(int value)
    {
        if (_angle_z != value)
        {
            _angle_z = value;
            update();
        }
    }

    void GLWidget::set_zoom_factor(double value)
    {
        if (_zoom != value)
        {
            _zoom = value;
            update();
        }
    }

    void GLWidget::set_trans_x(double value)
    {
        if (_trans_x != value)
        {
            _trans_x = value;
            update();
        }
    }

    void GLWidget::set_trans_y(double value)
    {
        if (_trans_y != value)
        {
            _trans_y = value;
            update();
        }
    }

    void GLWidget::set_trans_z(double value)
    {
        if (_trans_z != value)
        {
            _trans_z = value;
            update();
        }
    }



//    void GLWidget::set_v_alpha(double value)
//    {
//        if (_patch.getAlphaV() != value)
//        {
//            GLdouble u = _patch.getAlphaU();
//            GLdouble v = value;
//            _destroyPatch();
//            _patch = FirstOrderAlgebraicTrigonometricPatch3(u, v);
//            _createPatch();
//            update();
//        }
//    }

    void GLWidget::set_selected_row_index(int index)
    {
        if (_selected_row_index != index)
        {
            _selected_row_index = index;
            if (FOATPatches.PatchExists(_selected_foat_patch))
            {
                DCoordinate3 cp = FOATPatches.GetSelectedPatchSelectedControlPoint(_selected_foat_patch, _selected_row_index, _selected_column_index);
                emit selected_patch_cp_x_changed(cp[0]);
                emit selected_patch_cp_y_changed(cp[1]);
                emit selected_patch_cp_z_changed(cp[2]);
            }

            //update(); // lehet nem kell
        }
    }

    void GLWidget::set_selected_column_index(int index)
    {
        if (_selected_column_index != index)
        {
            _selected_column_index = index;
            if (FOATPatches.PatchExists(_selected_foat_patch))
            {
                DCoordinate3 cp = FOATPatches.GetSelectedPatchSelectedControlPoint(_selected_foat_patch, _selected_row_index, _selected_column_index);
                emit selected_patch_cp_x_changed(cp[0]);
                emit selected_patch_cp_y_changed(cp[1]);
                emit selected_patch_cp_z_changed(cp[2]);
            }

            //update(); // lehet nem kell
        }
    }

    void GLWidget::set_selected_point_x_value(double value)
    {
        if (FOATPatches.PatchExists(_selected_foat_patch))
        {
            if (FOATPatches.GetSelectedPatchSelectedControlPoint(_selected_foat_patch, _selected_row_index, _selected_column_index)[0] != value)
            {
                FOATPatches.Modify(0, _selected_foat_patch, value, _selected_row_index, _selected_column_index);
                update();
            }
        }

    }

    void GLWidget::set_selected_point_y_value(double value)
    {
        if (FOATPatches.PatchExists(_selected_foat_patch))
        {
            if (FOATPatches.GetSelectedPatchSelectedControlPoint(_selected_foat_patch, _selected_row_index, _selected_column_index)[1] != value)
            {
                FOATPatches.Modify(1, _selected_foat_patch, value, _selected_row_index, _selected_column_index);
                update();
            }
        }

    }
    void GLWidget::set_selected_point_z_value(double value)
    {
        if (FOATPatches.PatchExists(_selected_foat_patch))
        {
            if (FOATPatches.GetSelectedPatchSelectedControlPoint(_selected_foat_patch, _selected_row_index, _selected_column_index)[2] != value)
            {
                FOATPatches.Modify(2, _selected_foat_patch, value, _selected_row_index, _selected_column_index);
                update();
            }
        }

    }

    void GLWidget::insert_new_foat_patch()
    {
        constexpr int COUNT_MIN = 0;
        constexpr int COUNT_MAX = 5;

        random_device rd;
        default_random_engine eng(rd());
        uniform_real_distribution<float> distr(COUNT_MIN, COUNT_MAX);

        FOATPatches.InsertNewPatch(&materials[int(distr(eng))]);

        update();
    }

    void GLWidget::delete_selected_foat_patch()
    {
        FOATPatches.DeleteExistingPatch(_selected_foat_patch);
        update();
    }

    void GLWidget::set_selected_foat_patch(int value)
    {
        if (_selected_foat_patch != value)
        {
            _selected_foat_patch = value;

            if (FOATPatches.PatchExists(_selected_foat_patch))
            {
                DCoordinate3 cp = FOATPatches.GetSelectedPatchSelectedControlPoint(_selected_foat_patch, _selected_row_index, _selected_column_index);

                emit selected_patch_cp_x_changed(cp[0]);
                emit selected_patch_cp_y_changed(cp[1]);
                emit selected_patch_cp_z_changed(cp[2]);
            }
            FOATPatches.set_selected_patch_index(value);
            update();
        }

    }

    void GLWidget::set_u_alpha(double value)
    {
        if (FOATPatches.GetAlphaU() != value)
        {
            FOATPatches.SetAlphaU(value);
            update();
        }
    }

    void GLWidget::set_v_alpha(double value)
    {
        if (FOATPatches.GetAlphaV() != value)
        {
            FOATPatches.SetAlphaV(value);
            cout << "kijottem";
            update();
        }
    }

    void GLWidget::set_visibility_of_first_order_derivatives(bool value)
    {
        if (_foat_visibility_of_first_order_derivatives != value)
        {
            _foat_visibility_of_first_order_derivatives = value;
            update();
        }
    }

    void GLWidget::set_visibility_of_second_order_derivatives(bool value)
    {
        if (_foat_visibility_of_second_order_derivatives != value)
        {
            _foat_visibility_of_second_order_derivatives = value;
            update();
        }
    }

    void GLWidget::set_visibility_of_normal_vectors(bool value)
    {
        FOATPatches.set_visibility_of_normal_vectors(value);
        update();
    }

    void GLWidget::set_u_div_point_count(int value)
    {
        FOATPatches.set_u_div_point_count(value);
        update();
    }

    void GLWidget::set_u_iso_line_count(int value)
    {
        FOATPatches.set_u_iso_line_count(value);
        update();
    }

    void GLWidget::set_v_div_point_count(int value)
    {
        FOATPatches.set_v_div_point_count(value);
        update();
    }

    void GLWidget::set_v_iso_line_count(int value)
    {
        FOATPatches.set_v_iso_line_count(value);
        update();
    }

    void GLWidget::set_uv_scale(double value)
    {
        FOATPatches.set_uv_scale(value);
        update();
    }

    void GLWidget::set_normal_scale(double value)
    {
        FOATPatches.set_normal_scale(value);
        update();
    }


    void GLWidget::set_foat_arc_div_point(int value)
    {

        if ((GLuint)value != FOATCurves.GetDivPointCount())
        {
            FOATCurves.SetDivPointCount(value);
            FOATCurves.RenderArcsWithModifiedDivPointCount();
            update();
        }

    }

    void GLWidget::translate_foat_patch_along_x(double value)
    {
        if (FOATPatches.PatchExists(_selected_foat_patch))
        {
            value = value - _prev_patch_trans_x_value;
            _prev_patch_trans_x_value = value + _prev_patch_trans_x_value;

            /*for (GLuint i = 0; i < 4; ++i)
            {
                for (GLuint j = 0; j < 4; ++j)
                {
                    DCoordinate3 cp = FOATPatches.GetSelectedPatchSelectedControlPoint(_selected_foat_patch, i, j);
                    FOATPatches.ModifyPositionOfSelectedPatch(_selected_foat_patch, i, j, cp[0] + value, cp[1], cp[2]); // atirtam
                }
            }*/

            FOATPatches.TranslateSelectedPatch(_selected_foat_patch, 0, value);

            DCoordinate3 cp = FOATPatches.GetSelectedPatchSelectedControlPoint(_selected_foat_patch, _selected_row_index, _selected_column_index);
            emit selected_patch_cp_x_changed(cp[0]);

            update();
        }
    }

    void GLWidget::translate_foat_patch_along_y(double value)
    {
        if (FOATPatches.PatchExists(_selected_foat_patch))
        {
            value = value - _prev_patch_trans_y_value;
            _prev_patch_trans_y_value = value + _prev_patch_trans_y_value;

            /*for (GLuint i = 0; i < 4; ++i)
            {
                for (GLuint j = 0; j < 4; ++j)
                {
                    DCoordinate3 cp = FOATPatches.GetSelectedPatchSelectedControlPoint(_selected_foat_patch, i, j);
                    FOATPatches.ModifyPositionOfSelectedPatch(_selected_foat_patch, i, j, cp[0], cp[1] + value, cp[2]); // atirtam
                }
            }*/

            FOATPatches.TranslateSelectedPatch(_selected_foat_patch, 1, value);

            DCoordinate3 cp = FOATPatches.GetSelectedPatchSelectedControlPoint(_selected_foat_patch, _selected_row_index, _selected_column_index);
            emit selected_patch_cp_y_changed(cp[1]);

            update();
        }
    }

    void GLWidget::translate_foat_patch_along_z(double value)
    {
        if (FOATPatches.PatchExists(_selected_foat_patch))
        {
            value = value - _prev_patch_trans_z_value;
            _prev_patch_trans_z_value = value + _prev_patch_trans_z_value;

            /*for (GLuint i = 0; i < 4; ++i)
            {
                for (GLuint j = 0; j < 4; ++j)
                {
                    DCoordinate3 cp = FOATPatches.GetSelectedPatchSelectedControlPoint(_selected_foat_patch, i, j);
                    FOATPatches.ModifyPositionOfSelectedPatch(_selected_foat_patch, i, j, cp[0], cp[1], cp[2] + value); // atirtam
                }
            }*/

            FOATPatches.TranslateSelectedPatch(_selected_foat_patch, 2, value);

            DCoordinate3 cp = FOATPatches.GetSelectedPatchSelectedControlPoint(_selected_foat_patch, _selected_row_index, _selected_column_index);
            emit selected_patch_cp_z_changed(cp[2]);

            update();
        }

    }

    void GLWidget::set_foat_arc_alpha(double value)
    {

        if (FOATCurves.GetAlpha() != value)
        {
            _alpha = value;
            FOATCurves.SetAlphaAndRenderArcs(value);
            update();
        }
    }

    void GLWidget::translate_foat_arc_along_x(double value)
    {
       if (FOATCurves.ArcExists(_selected_foat_arc))
       {
           value = value - _prev_arc_trans_x_value;
           _prev_arc_trans_x_value = value + _prev_arc_trans_x_value;
           for (GLuint i = 0; i < 4; ++i)
           {
               double x = FOATCurves.GetSelectedArcSelectedControlPoint(_selected_foat_arc, i)[0] + value;
               double y = FOATCurves.GetSelectedArcSelectedControlPoint(_selected_foat_arc, i)[1];
               double z = FOATCurves.GetSelectedArcSelectedControlPoint(_selected_foat_arc, i)[2];
               FOATCurves.ModifyPositionOfSelectedArc(_selected_foat_arc, i, x, y, z);
           }

           update();
       }

    }

    void GLWidget::translate_foat_arc_along_y(double value)
    {
       if ( FOATCurves.ArcExists(_selected_foat_arc))
       {
           value = value - _prev_arc_trans_y_value;
           _prev_arc_trans_y_value = value + _prev_arc_trans_y_value;
           for (GLuint i = 0; i < 4; ++i)
           {
               double x = FOATCurves.GetSelectedArcSelectedControlPoint(_selected_foat_arc, i)[0];
               double y = FOATCurves.GetSelectedArcSelectedControlPoint(_selected_foat_arc, i)[1] + value;
               double z = FOATCurves.GetSelectedArcSelectedControlPoint(_selected_foat_arc, i)[2];
               FOATCurves.ModifyPositionOfSelectedArc(_selected_foat_arc, i, x, y, z);
           }

           update();
       }

    }

    void GLWidget::translate_foat_arc_along_z(double value)
    {
       if (FOATCurves.ArcExists(_selected_foat_arc))
       {
           value = value - _prev_arc_trans_z_value;
           _prev_arc_trans_z_value = value + _prev_arc_trans_z_value;
           for (GLuint i = 0; i < 4; ++i)
           {
               double x = FOATCurves.GetSelectedArcSelectedControlPoint(_selected_foat_arc, i)[0];
               double y = FOATCurves.GetSelectedArcSelectedControlPoint(_selected_foat_arc, i)[1];
               double z = FOATCurves.GetSelectedArcSelectedControlPoint(_selected_foat_arc, i)[2] + value;
               FOATCurves.ModifyPositionOfSelectedArc(_selected_foat_arc, i, x, y, z);
           }

           update();
       }

    }



    void GLWidget::set_foat_arc_cp_trans_x(double value)
    {
        if (FOATCurves.ArcExists(_selected_foat_arc) &&
                value != FOATCurves.GetSelectedArcSelectedControlPoint(_selected_foat_arc, _selected_foat_cp)[0])
        {
            double x = value;
            double y = FOATCurves.GetSelectedArcSelectedControlPoint(_selected_foat_arc, _selected_foat_cp)[1];
            double z = FOATCurves.GetSelectedArcSelectedControlPoint(_selected_foat_arc, _selected_foat_cp)[2];

            FOATCurves.ModifyPositionOfSelectedArc(_selected_foat_arc, _selected_foat_cp, x, y, z);
            update();

        }
    }

    void GLWidget::set_foat_arc_cp_trans_y(double value)
    {
        if (FOATCurves.ArcExists(_selected_foat_arc) &&
                value != FOATCurves.GetSelectedArcSelectedControlPoint(_selected_foat_arc, _selected_foat_cp)[1])
        {
            double x = FOATCurves.GetSelectedArcSelectedControlPoint(_selected_foat_arc, _selected_foat_cp)[0];
            double y = value;
            double z = FOATCurves.GetSelectedArcSelectedControlPoint(_selected_foat_arc, _selected_foat_cp)[2];

            FOATCurves.ModifyPositionOfSelectedArc(_selected_foat_arc, _selected_foat_cp, x, y, z);
            update();

        }
    }

    void GLWidget::set_foat_arc_cp_trans_z(double value)
    {
        if (FOATCurves.ArcExists(_selected_foat_arc) &&
                value != FOATCurves.GetSelectedArcSelectedControlPoint(_selected_foat_arc, _selected_foat_cp)[2])
        {
            double x = FOATCurves.GetSelectedArcSelectedControlPoint(_selected_foat_arc, _selected_foat_cp)[0];
            double y = FOATCurves.GetSelectedArcSelectedControlPoint(_selected_foat_arc, _selected_foat_cp)[1];
            double z = value;

            FOATCurves.ModifyPositionOfSelectedArc(_selected_foat_arc, _selected_foat_cp, x, y, z);
            update();

        }
    }


    void GLWidget::set_selected_foat_arc(int value)
    {
        if (_selected_foat_arc != (GLuint)value)
        {
            _selected_foat_arc = value;
            if (FOATCurves.ArcExists(_selected_foat_arc))
            {
                DCoordinate3 cp = FOATCurves.GetSelectedArcSelectedControlPoint(_selected_foat_arc, _selected_foat_cp);
                emit selected_arc_cp_x_changed(cp[0]);
                emit selected_arc_cp_y_changed(cp[1]);
                emit selected_arc_cp_z_changed(cp[2]);

                FOATCurves.set_selected_arc_index(value);
                update();
            }
        }
    }

    void GLWidget::set_selected_foat_cp(int value)
    {
        if (_selected_foat_cp != (GLuint)value)
        {
            _selected_foat_cp = value;

            if (FOATCurves.ArcExists(_selected_foat_arc))
            {
                DCoordinate3 cp = FOATCurves.GetSelectedArcSelectedControlPoint(_selected_foat_arc, _selected_foat_cp);
                emit selected_arc_cp_x_changed(cp[0]);
                emit selected_arc_cp_y_changed(cp[1]);
                emit selected_arc_cp_z_changed(cp[2]);
            }

        }
    }

    void GLWidget::delete_selected_arc()
    {
        FOATCurves.DeleteExistingArc(_selected_foat_arc);
        update();
    }

    void GLWidget::insert_new_foat_arc()
    {
        constexpr int FLOAT_MIN = 0;
        constexpr int FLOAT_MAX = 1;

        random_device rd;
        default_random_engine eng(rd());
        uniform_real_distribution<float> distr(FLOAT_MIN, FLOAT_MAX);

        Color4 *c = new Color4(distr(eng), distr(eng), distr(eng));
        FOATCurves.InsertNewArc(c);

        _selected_foat_arc = FOATCurves.GetArcCount() - 1;
        emit selected_arc_changed(_selected_foat_arc);
        DCoordinate3 cp = FOATCurves.GetSelectedArcSelectedControlPoint(_selected_foat_arc, _selected_foat_cp);
        emit selected_arc_cp_x_changed(cp[0]);
        emit selected_arc_cp_y_changed(cp[1]);
        emit selected_arc_cp_z_changed(cp[2]);

        FOATCurves.set_selected_arc_index(_selected_foat_arc);
        update();
    }

    void GLWidget::set_visibility_of_foat_arc_tangents(bool visibility)
    {
        if (_foat_arc_show_tangents != visibility)
        {
            _foat_arc_show_tangents = visibility;
            update();
        }
    }

    void GLWidget::set_visibility_of_foat_arc_acc_vectors(bool visibility)
    {
        if (_foat_arc_show_acceleration_vectors != visibility)
        {
            _foat_arc_show_acceleration_vectors = visibility;
            update();
        }
    }

    void GLWidget::set_foat_arc_scale(double value)
    {
        if (FOATCurves.GetScale() != value)
        {
            FOATCurves.SetScale(value);
            FOATCurves.RenderAllArcsScale();
            update();
        }
    }

    void GLWidget::show_merge_selected_foat_arcs_window()
    {
        _merge_window->show();
    }

    void GLWidget::show_continue_selected_foat_arcs_window()
    {
        _continue_window->show();
    }

    void GLWidget::show_join_selected_foat_arcs_window()
    {
        _join_window->show();
    }

    void GLWidget::merge_selected_foat_arcs()
    {
        GLuint arc1_index = (GLuint)_merge_window->select_arc_no_1_merge_spinbox->value();
        GLuint arc2_index = (GLuint)_merge_window->select_arc_no_2_merge_spinbox->value();

        if (arc1_index < FOATCurves.GetArcCount() && arc2_index < FOATCurves.GetArcCount())
        {
            FirstOrderAlgebraicTrigonometricCompositeCurve3::Direction arc1_direction = (FirstOrderAlgebraicTrigonometricCompositeCurve3::Direction)
                    _merge_window->select_direction_arc_no_1_merge_combobox->currentIndex();
            FirstOrderAlgebraicTrigonometricCompositeCurve3::Direction arc2_direction = (FirstOrderAlgebraicTrigonometricCompositeCurve3::Direction)
                    _merge_window->select_direction_arc_no_2_merge_combobox->currentIndex();
            FOATCurves.MergeExistingArcs(arc1_index, arc1_direction, arc2_index, arc2_direction);
            update();
        }
    }

    void GLWidget::set_shader_scale_factor(double value)
    {
        _shader.Enable();

        if (_shader_index == 3)
        {
            _scale_factor = value;
            _shader.SetUniformVariable1f("scale_factor", _scale_factor);
            update();
        }
    }

    void GLWidget::set_shader_smoothing(double value)
    {
        _shader.Enable();

        if (_shader_index == 3)
        {
            _smoothing = value;
            _shader.SetUniformVariable1f("smoothing", _smoothing);
            update();
        }
    }

    void GLWidget::set_shader_shading(double value)
    {
        _shader.Enable();

        if (_shader_index == 3)
        {
            _shading = value;
            _shader.SetUniformVariable1f("shading", _shading);
            update();
        }
    }

    void GLWidget::set_shader_index(int index)
    {
        if (_shader_index != index)
        {
            _shader_index = index;
            _initShader(_shader_index);
            update();
        }
    }

    GLWidget::~GLWidget()
    {
        switch(_test)
        {
            case PATCH:
                _destroyPatch();
                break;

            case ARC:
                _destroyArc();
                break;

            case FOAT_CURVES:
                _destroyArc();
                break;

            case FOAT_PATCHES:
                _destroyPatch2();
                break;

            default:
                break;
        }
    }


    void GLWidget::set_foat_join_index_0(int value)
    {
        if (_foat_join_index_0 != value)
        {
            _foat_join_index_0 = value;
        }
    }

    void GLWidget::set_foat_join_index_1(int value)
    {
        if (_foat_join_index_1 != value)
        {
            _foat_join_index_1 = value;
        }
    }

    void GLWidget::set_foat_join_direction_0(int value)
    {
         _foat_join_direction_0 = value;
    }

    void GLWidget::set_foat_join_direction_1(int value)
    {
        _foat_join_direction_1 = value;
    }

    void GLWidget::join_arcs()
    {
        vector<FirstOrderAlgebraicTrigonometricCompositeCurve3::Direction> directions {FirstOrderAlgebraicTrigonometricCompositeCurve3::Direction::LEFT,
                                                                                       FirstOrderAlgebraicTrigonometricCompositeCurve3::Direction::RIGHT};

        FOATCurves.JoinExistingArcs(_foat_join_index_0, directions[_foat_join_direction_0], _foat_join_index_1, directions[_foat_join_direction_1]);
        update();
    }

    void GLWidget::set_foat_join_patches_index_0(int value)
    {
        _foat_join_patches_index_0 = value;
    }

    void GLWidget::set_foat_join_patches_index_1(int value)
    {
        _foat_join_patches_index_1 = value;
    }

    void GLWidget::set_foat_join_patches_direction_0(int value)
    {
        _foat_join_patches_direction_0 = value;
    }

    void GLWidget::set_foat_join_patches_direction_1(int value)
    {
        _foat_join_patches_direction_1 = value;
    }


    void GLWidget::join_patches()
    {
        vector<FirstOrderAlgebraicTrigonometricCompositePatch3::Direction> directions {FirstOrderAlgebraicTrigonometricCompositePatch3::Direction::N,
                                                                                         FirstOrderAlgebraicTrigonometricCompositePatch3::Direction::NW,
                                                                                         FirstOrderAlgebraicTrigonometricCompositePatch3::Direction::W,
                                                                                         FirstOrderAlgebraicTrigonometricCompositePatch3::Direction::SW,
                                                                                         FirstOrderAlgebraicTrigonometricCompositePatch3::Direction::S,
                                                                                         FirstOrderAlgebraicTrigonometricCompositePatch3::Direction::SE,
                                                                                         FirstOrderAlgebraicTrigonometricCompositePatch3::Direction::E,
                                                                                         FirstOrderAlgebraicTrigonometricCompositePatch3::Direction::NE};

        FOATPatches.JoinExistingPatches(_foat_join_patches_index_0, directions[_foat_join_patches_direction_0], _foat_join_patches_index_1, directions[_foat_join_patches_direction_1]);
        update();
    }

    void GLWidget::show_join_selected_foat_patches_window()
    {
        _join_patch_window->show();
    }

    void GLWidget::show_merge_patches_window()
    {
        _merge_patch_window->show();
    }

    void GLWidget::merge_selected_patches()
    {
        GLuint patch1_index = (GLuint)_merge_patch_window->select_patch_no_1_merge_spinbox->value();
        GLuint patch2_index = (GLuint)_merge_patch_window->select_patch_no_2_merge_spinbox->value();

        if (FOATPatches.PatchExists(patch1_index) && FOATPatches.PatchExists(patch2_index))
        {
            FirstOrderAlgebraicTrigonometricCompositePatch3::Direction patch1_direction = (FirstOrderAlgebraicTrigonometricCompositePatch3::Direction)
                    _merge_patch_window->select_direction_patch_no_1_merge_combobox->currentIndex();
            FirstOrderAlgebraicTrigonometricCompositePatch3::Direction patch2_direction = (FirstOrderAlgebraicTrigonometricCompositePatch3::Direction)
                    _merge_patch_window->select_direction_patch_no_2_merge_combobox->currentIndex();
            FOATPatches.MergeExistingPatches(patch1_index, patch1_direction, patch2_index, patch2_direction);
            update();
        }
    }

    void GLWidget::continue_selected_foat_arc()
    {
        GLuint selected_arc = (GLuint)_continue_window->selected_arc_spin_box->value();
        FirstOrderAlgebraicTrigonometricCompositeCurve3::Direction direction = (FirstOrderAlgebraicTrigonometricCompositeCurve3::Direction)
                _continue_window->direction_combo_box->currentIndex();

        if (FOATCurves.ArcExists(selected_arc))
        {
            FOATCurves.ContinueExisitingArc(selected_arc, direction);
            update();
        }
    }

    void GLWidget::show_continue_patch_window()
    {
        _continue_patch_window->show();
    }

    void GLWidget::continue_selected_foat_patch()
    {
        GLuint selected_patch = (GLuint)_continue_patch_window->selected_patch_spin_box->value();
        FirstOrderAlgebraicTrigonometricCompositePatch3::Direction direction = (FirstOrderAlgebraicTrigonometricCompositePatch3::Direction)
                _continue_patch_window->direction_combo_box->currentIndex();

        constexpr int COUNT_MIN = 0;
        constexpr int COUNT_MAX = 6;

        random_device rd;
        default_random_engine eng(rd());
        uniform_real_distribution<float> distr(COUNT_MIN, COUNT_MAX);

        FOATPatches.ContinueExistingPatch(selected_patch, direction, &materials[int(distr(eng))]);
        update();
    }

    void GLWidget::set_texture_nr(int index)
    {cout << "Szia" << endl;
        QString _texture_name;
        switch(index)
                {
                    case 1: _texture_name = "Textures\\periodic_texture_00.jpg"; break;
                    case 2: _texture_name = "Textures\\periodic_texture_01.jpg"; break;
                    case 3: _texture_name = "Textures\\periodic_texture_02.jpg"; break;
                    case 4: _texture_name = "Textures\\periodic_texture_03.jpg"; break;
                    case 5: _texture_name = "Textures\\periodic_texture_04.jpg"; break;
                    case 6: _texture_name = "Textures\\periodic_texture_05.jpg"; break;
                    case 7: _texture_name = "Textures\\periodic_texture_06.jpg"; break;
                    case 8: _texture_name = "Textures\\periodic_texture_07.jpg"; break;
                    case 9: _texture_name = "Textures\\periodic_texture_08.jpg"; break;
                    case 10: _texture_name = "Textures\\periodic_texture_09.jpg"; break;
                default: break;
                }

        QOpenGLTexture *texture = new QOpenGLTexture(QImage(_texture_name).mirrored());
        texture->setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);
        texture->setMagnificationFilter(QOpenGLTexture::Linear);

        FOATPatches.SetTextureOfSelectedPatch(_selected_foat_patch, texture);
        update();
    }

    void GLWidget::set_show_texture(bool value)
    {
        _show_texture = value;
        FOATPatches.SetShowTexture(value);
        update();
    }

    void GLWidget::set_material_nr(int value)
    {
        FOATPatches.SetMaterialOfSelectedPatch(_selected_foat_patch, &materials[value]);
        update();
    }

    void GLWidget::_renderPatchColors() // atirtam3
    {
        FOATPatches.RenderColors();
    }

    void GLWidget::save()
    {
        FOATPatches.save();
    }

    void GLWidget::load()
    {
        FOATPatches.load();

        DCoordinate3 cp = FOATPatches.GetSelectedPatchSelectedControlPoint(_selected_foat_patch, _selected_row_index, _selected_column_index);
        emit selected_patch_cp_x_changed(cp[0]);
        emit selected_patch_cp_y_changed(cp[1]);
        emit selected_patch_cp_z_changed(cp[2]);

        update();
    }

    void GLWidget::set_render_type(int value)
    {
        if(value == 0)
        {
            _test = FOAT_CURVES;
        }
        else
        {
            _test = FOAT_PATCHES;
        }
        update();
    }

}

