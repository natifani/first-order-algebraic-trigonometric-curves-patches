#ifndef FIRSTORDERALGEBRAICTRIGONOMETRICCOMPOSITEPATCH3_H
#define FIRSTORDERALGEBRAICTRIGONOMETRICCOMPOSITEPATCH3_H

#include <Core/Materials.h>
#include "FirstOrderAlgebraicTrigonometricPatches3.h"
#include <iostream>
#include <Core/ShaderPrograms.h>
#include <QOpenGLTexture>

using namespace std;

namespace cagd
{
    class FirstOrderAlgebraicTrigonometricCompositePatch3
    {
    public:
        enum Direction{N, NW, W, SW, S, SE, E, NE};
        // N = eszak, NW = eszaknyugat, W = nyugat, SW = delnyugat, S = del, SE = delkelet, E = kelet, NE = eszakkelet

        class PatchAttributes
        {
        public:
            FirstOrderAlgebraicTrigonometricPatch3          *patch;
            TriangulatedMesh3   *image;
            Material            *material; // use pointers to pre-defined materials
            QOpenGLTexture      *texture;  // use pointers to pre-defined textures
            ShaderProgram       *shader;   // use pointers to pre-defined shader programs
            RowMatrix<GenericCurve3*>* _uIsoparametricLines;
            RowMatrix<GenericCurve3*>* _vIsoparametricLines;

            // other attributes
            // ...

            PatchAttributes   *neighbours[8];
            GLuint            neighbours_index[8]; // uj
            Direction         connection_type[8];

            PatchAttributes()
            {
                _uIsoparametricLines = nullptr;
                _vIsoparametricLines = nullptr;
                patch = nullptr;
                image = nullptr;
                for (GLuint i = 0; i < 8; ++i)
                {
                    neighbours[i] = nullptr;
                    neighbours_index[i] = -1; // uj
                }

                material = &MatFBBrass;
                texture = nullptr;
                shader = nullptr;
            }

            PatchAttributes(const PatchAttributes &rhs)
            {
                patch = new FirstOrderAlgebraicTrigonometricPatch3(rhs.patch->getAlphaU(), rhs.patch->getAlphaV());
                for (GLuint i = 0; i < 4; ++i)
                {
                    for (GLuint j = 0; j < 4; ++j)
                    {
                        (*patch)(i, j) = (*rhs.patch)(i, j);
                    }
                }
                patch->UpdateVertexBufferObjectsOfData();
                material = rhs.material;

                for (GLuint i = 0; i < 8; ++i)
                {
                    neighbours[i] = rhs.neighbours[i];
                }

                _uIsoparametricLines = rhs._uIsoparametricLines;

                for (GLuint i = 0; i <_uIsoparametricLines->GetColumnCount(); ++i)
                {
                   (*_uIsoparametricLines)[i]->UpdateVertexBufferObjects();
                }

                for (GLuint i = 0; i < _vIsoparametricLines->GetColumnCount(); ++i)
                {
                    (*_vIsoparametricLines)[i]->UpdateVertexBufferObjects();
                }

                image = patch->GenerateImage(30, 30);
                image->UpdateVertexBufferObjects();

                // texture es shader ...
            }

            ~PatchAttributes()
            {
                if (patch){
                    delete patch; patch = nullptr;
                }

                if (image){
                    delete image; image = 0;
                }

//                if (material){
//                    delete material; material = nullptr;
//                }

//                for (GLuint i = 0; i < 8; ++i)
//                {
//                    if (neighbours[i])
//                    {
//                        delete neighbours[i]; neighbours[i] = nullptr;
//                    }
//                }

                if (_uIsoparametricLines)
                {
                    for (GLuint i = 0; i < _uIsoparametricLines->GetColumnCount(); ++i){
                        if ((*_uIsoparametricLines)[i])
                        {
                            delete (*_uIsoparametricLines)[i];
                            (*_uIsoparametricLines)[i] = nullptr;
                        }
                    }
                    delete _uIsoparametricLines; _uIsoparametricLines = nullptr;
                }

                if (_vIsoparametricLines)
                {
                    for (GLuint i = 0; i < _vIsoparametricLines->GetColumnCount(); ++i){
                        if ((*_vIsoparametricLines)[i])
                        {
                            delete (*_vIsoparametricLines)[i];
                            (*_vIsoparametricLines)[i] = nullptr;
                        }
                    }
                    delete _vIsoparametricLines; _vIsoparametricLines = nullptr;
                }


                // texture es shader ...
            }
            // ctor, copy ctor, assignment operator, dtor (they are required by the std::vector!)
        };

    protected:
        GLuint                     _uDivPointCount = 30;
        GLuint                     _uIsoLineCount = 5;
        GLuint                     _vDivPointCount = 30;
        GLuint                     _vIsoLineCount = 5;
        GLdouble                   _uvScale = 1.0;
        GLdouble                   _normalScale = 1.0;
        bool                       _visibility_of_normal_vectors = false;
        GLuint                       _patch_count = 0;
        GLdouble                     _alpha[2];     // possible (shared) shape variables in directions u and v
        std::vector<PatchAttributes> _attributes;
        GLuint _selected_patch_index = 0;
        bool                       _show_texture = false;

    public:
        // special/default ctor
        FirstOrderAlgebraicTrigonometricCompositePatch3(GLdouble u_alpha = PI / 2.0, GLdouble v_alpha = PI / 2.0, GLuint minimial_patch_count_to_be_reserved = 500);

        // operations
        GLboolean InsertNewPatch(Material *material, GLenum usage_flag = GL_STATIC_DRAW);
        GLboolean DeleteExistingPatch(GLuint index);
        void DeleteAllPatches();
        GLboolean ContinueExistingPatch(GLuint index, Direction direction, Material *material);
        GLboolean InsertNewPatch2(DCoordinate3 *coord, Material *material, GLenum usage_flag = GL_STATIC_DRAW);
        GLboolean JoinExistingPatches(GLuint index_0, Direction direction_0, GLuint index_1, Direction direction_1);
        GLboolean MergeExistingPatches(GLuint index_0, Direction direction_0, GLuint index_1, Direction direction_1);

        GLboolean RenderAllPatches(GLuint order, GLenum render_mode = GL_LINE_STRIP) const;
        GLboolean RenderSelectedPatch(GLuint index, GLuint order, GLenum render_mode = GL_LINE_STRIP) const;
        GLboolean PatchExists(GLuint index) const;
        DCoordinate3 GetSelectedPatchSelectedControlPoint(GLuint selected_foat_patch, GLuint row_index, GLuint column_index);
        void ModifyPositionOfSelectedPatch(GLuint selected_foat_patch, GLuint row_index, GLuint column_index, GLdouble x, GLdouble y, GLdouble z);

        GLboolean GenerateImageOfSelectedPatches(GLuint index_0, GLuint index_1);
        void ModifyUV(GLuint value, GLuint index); // foat2
        void Modify(int coord, int index, double value, int selected_row_index, int selected_column_index);

        void set_u_div_point_count(GLuint value);
        void set_u_iso_line_count(GLuint value);
        void set_v_div_point_count(GLuint value);
        void set_v_iso_line_count(GLuint value);
        void set_uv_scale(GLdouble value);
        void set_normal_scale(GLdouble value);
        void set_visibility_of_normal_vectors(bool value);
        // other setters and getters
        GLdouble GetAlphaU() const;
        GLdouble GetAlphaV() const;

        GLboolean SetAlphaOfPatches(int index);
        GLboolean SetAlphaHelper(int i);
        GLboolean SetAlphaU(double value);
        GLboolean SetAlphaV(double value);

        void TranslateSelectedPatch(int index, int coord, double value);
        void set_selected_patch_index(GLuint index);

        void SetTextureOfSelectedPatch(GLuint index, QOpenGLTexture *texture);
        void SetMaterialOfSelectedPatch(GLuint index, Material *material);
        void SetShowTexture(bool value);
        GLboolean RenderColors() const;
        void MoveControlPoint(int index, int r, int c, bool &new_image_1, Direction &direction_1);


        void load();
        void save();

    };
}

#endif // FIRSTORDERALGEBRAICTRIGONOMETRICCOMPOSITESURFACES3_H
