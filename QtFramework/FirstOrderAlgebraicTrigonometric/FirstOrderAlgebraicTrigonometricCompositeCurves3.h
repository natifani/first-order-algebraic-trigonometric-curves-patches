#ifndef FIRSTORDERALGEBRAICTRIGONOMETRICCOMPOSITECURVES3_H
#define FIRSTORDERALGEBRAICTRIGONOMETRICCOMPOSITECURVES3_H

#include <Core/Colors4.h>
#include "FirstOrderAlgebraicTrigonometricArcs3.h"
using namespace std;

namespace cagd
{
    class FirstOrderAlgebraicTrigonometricCompositeCurve3
    {
    public:
        enum Direction{LEFT, RIGHT};

        class ArcAttributes
        {
        public:
            FirstOrderAlgebraicTrigonometricArc3 *arc;
            GenericCurve3   *image;
            Color4          *color;           // use pointers to pre-defined colors, e.g., Color4 *color = &red;
            // other arc attributes
            // ...

            ArcAttributes   *previous, *next; // pointers to neighboring arc attributes
            Direction       previous_connection_type, next_connection_type;

            ArcAttributes()
            {
                arc = nullptr;
                image = nullptr;
                previous = nullptr;
                next = nullptr;
                color = new Color4(0.0, 0.0, 1.0);
            }

            ArcAttributes(const ArcAttributes &rhs)
            {
                this->arc = new FirstOrderAlgebraicTrigonometricArc3(rhs.arc->GetAlpha());
                (*arc)[0] = (*rhs.arc)[0];
                (*arc)[1] = (*rhs.arc)[1];
                (*arc)[2] = (*rhs.arc)[2];
                (*arc)[3] = (*rhs.arc)[3];
                arc->UpdateVertexBufferObjectsOfData();
                this->color = rhs.color;
                next = rhs.next;
                previous = rhs.previous;

                image = arc->GenerateImage(2, 100);
                image->UpdateVertexBufferObjects();
            }

            ~ArcAttributes()
            {
                if (arc){
                    delete arc, arc = 0;
                }

                if (image){
                    delete image, image = 0;
                }
            }

            // ctor, copy ctor, assignment operator, dtor (they are required by the std::vector!)
        };

    protected:
        GLuint                     _arc_count = 0;
        GLdouble                   _alpha;               // possible (shared) shape parameter
        GLuint                     _arc_div_point_count = 100;
        GLuint                     _arc_max_order_of_derivatives = 2;
        GLdouble                   _arc_scale = 1.0;
        vector<ArcAttributes> _attributes;          // sequence of arc attributes
        GLuint _selected_arc_index = 0;
        GLuint _selected_cp = 0;
    public:
        // special/default ctor
        FirstOrderAlgebraicTrigonometricCompositeCurve3(GLdouble alpha = PI / 2.0, GLuint minimal_arc_count_to_be_reserved = 1000);

        GLboolean InsertNewArc(Color4 *color, GLenum usage_flag = GL_STATIC_DRAW);
        GLboolean DeleteExistingArc(GLuint index);
        GLboolean ContinueExisitingArc(GLuint index, Direction direction);
        GLboolean JoinExistingArcs(GLuint index_0, Direction direction_0, GLuint index_1, Direction direction_1);
        GLboolean MergeExistingArcs(GLuint index_0, Direction direction_0, GLuint index_1, Direction direction_1);
        GLboolean InsertLine(Color4 *color, GLuint max_order_of_derivatives, GLuint div_point_count, GLenum usage_flag = GL_STATIC_DRAW);
        GLboolean InsertNewArc2(DCoordinate3 *pontok, Color4 *color, GLuint max_order_of_derivatives, GLuint div_point_count, GLenum usage_flag = GL_STATIC_DRAW);


        void DeleteAllArcs();
        GLboolean RenderAllArcs(GLuint order, GLenum render_mode) const;
        GLboolean RenderSelectedArc(GLuint index, GLuint order, GLenum render_mode) const;
        void RenderAllArcsScale();
        GLboolean GenerateImageOfSelectedArcs(GLuint index_0, GLuint index_1);
        GLboolean ArcExists(GLuint index) const;

        void ModifyPositionOfSelectedArc(GLuint index, GLuint cp_index, double x, double y, double z, GLenum usage_flag = GL_STATIC_DRAW);

        // other setters and getters
        DCoordinate3 GetSelectedArcSelectedControlPoint(GLuint index, GLuint cp_index) const;
        void setAlpha(GLdouble alpha);
        GLdouble GetAlpha() const;
        GLboolean SetAlpha(GLdouble alpha);
        GLboolean SetScale(GLdouble scale);
        GLdouble GetScale() const;


        GLuint GetDivPointCount() const;
        GLboolean SetDivPointCount(GLuint div_point_count);

        GLuint GetArcCount() const;

        GLboolean SetAlphaOfArcs();
        void SetAlphaAndRenderArcs(double alpha, GLenum usage_flag = GL_STATIC_DRAW);
        void RenderArcsWithModifiedDivPointCount(GLenum usage_flag = GL_STATIC_DRAW);

        void set_selected_arc_index(GLuint index);
        void set_selected_cp(GLuint index);

    };
}

#endif // FIRSTORDERALGEBRAICTRIGONOMETRICCOMPOSITECURVES3_H
