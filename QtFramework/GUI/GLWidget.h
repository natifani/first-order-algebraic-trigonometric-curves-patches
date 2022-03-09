#pragma once

#include <GL/glew.h>
#include <QOpenGLWidget>

#include "Core/GenericCurves3.h"
#include "Core/TriangulatedMeshes3.h"
#include "Core/ShaderPrograms.h"
#include "FirstOrderAlgebraicTrigonometric/FirstOrderAlgebraicTrigonometricPatches3.h"
#include "FirstOrderAlgebraicTrigonometric/FirstOrderAlgebraicTrigonometricArcs3.h"
#include "FirstOrderAlgebraicTrigonometric/FirstOrderAlgebraicTrigonometricCompositeCurves3.h"
#include "FirstOrderAlgebraicTrigonometric/FirstOrderAlgebraicTrigonometricCompositePatch3.h"
#include "GUI/mergewindow.h"
#include "GUI/continuewindow.h"
#include "GUI/joinwindow.h"
#include "GUI/joinpatchwindow.h"
#include "GUI/mergepatchwindow.h"
#include "GUI/continuepatchwindow.h"

namespace cagd
{
    class GLWidget: public QOpenGLWidget
    {
        Q_OBJECT

    private:
        MergeWindow *_merge_window; //foat
        ContinueWindow *_continue_window; //foat
        JoinWindow *_join_window; //foat
        JoinPatchWindow *_join_patch_window;
        MergePatchWindow *_merge_patch_window;
        ContinuePatchWindow * _continue_patch_window;

        // variables defining the projection matrix
        double       _aspect;            // aspect ratio of the rendering window
        double       _fovy;              // field of view in direction y
        double       _z_near, _z_far;    // distance of near and far clipping planes

        // variables defining the model-view matrix
        double       _eye[3], _center[3], _up[3];

        // variables needed by transformations
        int         _angle_x, _angle_y, _angle_z;
        double      _zoom;
        double      _trans_x, _trans_y, _trans_z;

        // 0. test
        void _simpleTriangle();

        // 1. patch
        FirstOrderAlgebraicTrigonometricCompositePatch3 FOATPatches;
        int _selected_row_index = 0;
        int _selected_column_index = 0;
        double _selected_point_x_value = 0;
        double _selected_point_y_value = 0;
        double _selected_point_z_value = 0;
        RowMatrix<GLdouble> u_knot_vector;
        ColumnMatrix<GLdouble> v_knot_vector;
        bool _foat_visibility_of_first_order_derivatives = false;
        bool _foat_visibility_of_second_order_derivatives = false;
        GLdouble _prev_patch_trans_x_value = 0.0;
        GLdouble _prev_patch_trans_y_value = 0.0;
        GLdouble _prev_patch_trans_z_value = 0.0;
        int _selected_foat_patch = 0;
        int _selected_foat_cp2 = 0;
        vector<Material> materials {MatFBBrass, MatFBSilver,MatFBEmerald,MatFBPearl,MatFBRuby,MatFBTurquoise};
        bool _createPatch2();
        bool _renderPatch2();
        void _destroyPatch2();

        void _createPatch();
        void _renderPatch();
        void _destroyPatch();


        // 2. arc
        GLdouble _alpha = PI / 2.0;
        GLdouble _n = 4;
        FirstOrderAlgebraicTrigonometricCompositeCurve3 FOATCurves;
        GLuint _selected_foat_arc = 0;
        GLuint _selected_foat_cp = 0;
        bool _foat_arc_show_tangents = false;
        bool _foat_arc_show_acceleration_vectors = false;
        GLdouble _prev_arc_trans_x_value = 0.0;
        GLdouble _prev_arc_trans_y_value = 0.0;
        GLdouble _prev_arc_trans_z_value = 0.0;
        double _foat_arc_scale = 1.0;

        bool _createArc();  // foat
        bool _renderArc();  // foat
        void _destroyArc(); // foat

        // 3. shader
        ShaderProgram _shader;
        GLfloat _scale_factor = 1.0f;
        GLfloat _smoothing = 1.0f;
        GLfloat _shading = 1.0f;
        int _shader_index;

        void _initShader(int index = 0);
        bool _renderShader();

        void modify(int coord);

        // u-v iso lines
        bool _visibilityOfFirstOrderDerivatives = false;
        bool _visibilityOfSecondOrderDerivatives = false;
        int _uDivPointCount = 100;
        int _uIsoLineCount = 5;
        int _vDivPointCount = 100;
        int _vIsoLineCount = 5;
        double _uvScale = 1.0;
        bool _visibilityOfNormalVectors = false;
        double _normalScale = 1.0;

        void _modifyUV(int value);

        // FOAT_JOIN
        int _foat_join_index_0 = 0;
        int _foat_join_index_1 = 1;
        int _foat_join_direction_0 = 0;     /// 1 = RIGHT = RED
        int _foat_join_direction_1 = 0;     /// 0 = LEFT = GREEN

        int _foat_join_patches_index_0 = 0;
        int _foat_join_patches_index_1 = 1;
        int _foat_join_patches_direction_0 = 0;
        int _foat_join_patches_direction_1 = 0;

        bool _show_texture = false;
        void _renderPatchColors();

        // your other declarations

        enum TestType{SIMPLE_TRIANGLE, PATCH, ARC, SHADER, FOAT_CURVES, FOAT_PATCHES};
        TestType _test = FOAT_CURVES;

    public:
        // special and default constructor
        // the format specifies the properties of the rendering window
        GLWidget(QWidget* parent = 0, MergeWindow* merge_window = nullptr, ContinueWindow* continue_window = nullptr, JoinWindow* join_window = nullptr, JoinPatchWindow *join_patch_window = nullptr, MergePatchWindow *merge_patch_window = nullptr, ContinuePatchWindow *continue_patch_window = nullptr);

        // destructor
        ~GLWidget();

        // redeclared virtual functions
        void initializeGL();
        void paintGL();
        void resizeGL(int w, int h);

    public slots:
        // public event handling methods/slots
        void set_angle_x(int value);
        void set_angle_y(int value);
        void set_angle_z(int value);

        void set_zoom_factor(double value);

        void set_trans_x(double value);
        void set_trans_y(double value);
        void set_trans_z(double value);

        // 2. arc
        void set_foat_arc_alpha(double value);
        void set_foat_arc_div_point(int value);
        void set_visibility_of_foat_arc_tangents(bool visibility);
        void set_visibility_of_foat_arc_acc_vectors(bool visibility);
        void set_selected_foat_arc(int value);
        void set_selected_foat_cp(int value);
        void set_foat_arc_cp_trans_x(double value);
        void set_foat_arc_cp_trans_y(double value);
        void set_foat_arc_cp_trans_z(double value);
        void delete_selected_arc();
        void insert_new_foat_arc();
        void set_foat_arc_scale(double value);
        void translate_foat_arc_along_x(double value);
        void translate_foat_arc_along_y(double value);
        void translate_foat_arc_along_z(double value);
        void show_merge_selected_foat_arcs_window();
        void show_join_selected_foat_arcs_window();
        void show_continue_selected_foat_arcs_window();
        void merge_selected_foat_arcs();

        void show_merge_patches_window();
        void merge_selected_patches();


        // 3. shader
        void set_shader_scale_factor(double value);
        void set_shader_smoothing(double value);
        void set_shader_shading(double value);
        void set_shader_index(int index);

        // 1. patch
        void set_u_alpha(double value);
        void set_v_alpha(double value);
        void set_selected_row_index(int index);
        void set_selected_column_index(int index);
        void set_selected_point_x_value(double value);
        void set_selected_point_y_value(double value);
        void set_selected_point_z_value(double value);
        void insert_new_foat_patch();
        void delete_selected_foat_patch();
        void set_selected_foat_patch(int value);
        void set_visibility_of_first_order_derivatives(bool value);
        void set_visibility_of_second_order_derivatives(bool value);
        void set_u_div_point_count(int value);
        void set_u_iso_line_count(int value);
        void set_v_div_point_count(int value);
        void set_v_iso_line_count(int value);
        void set_uv_scale(double value);
        void set_visibility_of_normal_vectors(bool value);
        void set_normal_scale(double value);
        void translate_foat_patch_along_x(double value);
        void translate_foat_patch_along_y(double value);
        void translate_foat_patch_along_z(double value);

        void join_arcs();
        void set_foat_join_index_0(int value);
        void set_foat_join_index_1(int value);
        void set_foat_join_direction_0(int value);
        void set_foat_join_direction_1(int value);

        void join_patches();
        void set_foat_join_patches_index_0(int value);
        void set_foat_join_patches_index_1(int value);
        void set_foat_join_patches_direction_0(int value);
        void set_foat_join_patches_direction_1(int value);

        void show_join_selected_foat_patches_window();

        void continue_selected_foat_arc();
        void show_continue_patch_window();
        void continue_selected_foat_patch();

        void set_texture_nr(int);
        void set_show_texture(bool);
        void set_material_nr(int);

        void save();
        void load();

        void set_render_type(int);


    signals:
            void selected_arc_changed(int);
            void selected_patch_cp_x_changed(double);
            void selected_patch_cp_y_changed(double);
            void selected_patch_cp_z_changed(double);
            void selected_patch_trans_x(double);
            void selected_patch_trans_y(double);
            void selected_patch_trans_z(double);

            void selected_arc_cp_x_changed(double);
            void selected_arc_cp_y_changed(double);
            void selected_arc_cp_z_changed(double);
            void selected_arc_trans_x(double);
            void selected_arc_trans_y(double);
            void selected_arc_trans_z(double);
    };
}
