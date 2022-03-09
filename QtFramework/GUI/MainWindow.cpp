#include "MainWindow.h"

namespace cagd
{
    MainWindow::MainWindow(QWidget *parent): QMainWindow(parent)
    {
        setupUi(this);

    /*

      the structure of the main window's central widget

     *---------------------------------------------------*
     |                 central widget                    |
     |                                                   |
     |  *---------------------------*-----------------*  |
     |  |     rendering context     |   scroll area   |  |
     |  |       OpenGL widget       | *-------------* |  |
     |  |                           | | side widget | |  |
     |  |                           | |             | |  |
     |  |                           | |             | |  |
     |  |                           | *-------------* |  |
     |  *---------------------------*-----------------*  |
     |                                                   |
     *---------------------------------------------------*

    */
        _side_widget = new SideWidget(this);

        _scroll_area = new QScrollArea(this);
        _scroll_area->setWidget(_side_widget);
        _scroll_area->setSizePolicy(_side_widget->sizePolicy());
        _scroll_area->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

        _merge_window = new MergeWindow(this);
        _continue_window = new ContinueWindow(this);
        _join_window = new JoinWindow(this);
        _join_patch_window = new JoinPatchWindow(this);
        _merge_patch_window = new MergePatchWindow(this);
        _continue_patch_window = new ContinuePatchWindow(this);
        _gl_widget = new GLWidget(this, _merge_window, _continue_window, _join_window, _join_patch_window, _merge_patch_window, _continue_patch_window);

        centralWidget()->setLayout(new QHBoxLayout());
        centralWidget()->layout()->addWidget(_gl_widget);
        centralWidget()->layout()->addWidget(_scroll_area);

        // creating a signal slot mechanism between the rendering context and the side widget
        connect(_side_widget->rotate_x_slider, SIGNAL(valueChanged(int)), _gl_widget, SLOT(set_angle_x(int)));
        connect(_side_widget->rotate_y_slider, SIGNAL(valueChanged(int)), _gl_widget, SLOT(set_angle_y(int)));
        connect(_side_widget->rotate_z_slider, SIGNAL(valueChanged(int)), _gl_widget, SLOT(set_angle_z(int)));

        connect(_side_widget->zoom_factor_spin_box, SIGNAL(valueChanged(double)), _gl_widget, SLOT(set_zoom_factor(double)));

        connect(_side_widget->trans_x_spin_box, SIGNAL(valueChanged(double)), _gl_widget, SLOT(set_trans_x(double)));
        connect(_side_widget->trans_y_spin_box, SIGNAL(valueChanged(double)), _gl_widget, SLOT(set_trans_y(double)));
        connect(_side_widget->trans_z_spin_box, SIGNAL(valueChanged(double)), _gl_widget, SLOT(set_trans_z(double)));

        // 3. shader
        connect(_side_widget->shader_scale_spin_box, SIGNAL(valueChanged(double)),_gl_widget, SLOT(set_shader_scale_factor(double)));
        connect(_side_widget->shader_smooth_spin_box, SIGNAL(valueChanged(double)),_gl_widget, SLOT(set_shader_smoothing(double)));
        connect(_side_widget->shader_shade_spin_box, SIGNAL(valueChanged(double)),_gl_widget, SLOT(set_shader_shading(double)));
        connect(_side_widget->shader_name_combo_box, SIGNAL(currentIndexChanged(int)), _gl_widget, SLOT(set_shader_index(int)));


        // FOATCurves - INNENTO LEFELE
        connect(_side_widget->arc_alpha_spinbox, SIGNAL(valueChanged(double)), _gl_widget, SLOT(set_foat_arc_alpha(double)));
        connect(_side_widget->arc_div_point_spinbox, SIGNAL(valueChanged(int)), _gl_widget, SLOT(set_foat_arc_div_point(int)));
        connect(_side_widget->selected_foat_arc_spinBox, SIGNAL(valueChanged(int)), _gl_widget, SLOT(set_selected_foat_arc(int)));
        connect(_side_widget->arc_cp_spinbox, SIGNAL(valueChanged(int)), _gl_widget, SLOT(set_selected_foat_cp(int)));
        connect(_side_widget->arc_cp_translate_x_spinbox, SIGNAL(valueChanged(double)), _gl_widget, SLOT(set_foat_arc_cp_trans_x(double)));
        connect(_side_widget->arc_cp_translate_y_spinbox, SIGNAL(valueChanged(double)), _gl_widget, SLOT(set_foat_arc_cp_trans_y(double)));
        connect(_side_widget->arc_cp_translate_z_spinbox, SIGNAL(valueChanged(double)), _gl_widget, SLOT(set_foat_arc_cp_trans_z(double)));
        connect(_side_widget->delete_selected_arc_button, SIGNAL(clicked()), _gl_widget, SLOT(delete_selected_arc()));
        connect(_side_widget->show_arc_tangents_checkbox, SIGNAL(clicked(bool)), _gl_widget, SLOT(set_visibility_of_foat_arc_tangents(bool)));
        connect(_side_widget->show_arc_acc_vectors_checkbox, SIGNAL(clicked(bool)), _gl_widget, SLOT(set_visibility_of_foat_arc_acc_vectors(bool)));
        connect(_side_widget->arc_scale_spinbox, SIGNAL(valueChanged(double)), _gl_widget, SLOT(set_foat_arc_scale(double)));
        connect(_side_widget->insert_arc_button, SIGNAL(clicked()), _gl_widget, SLOT(insert_new_foat_arc()));
        connect(_side_widget->merge_selected_arcs_button, SIGNAL(clicked()), _gl_widget, SLOT(show_merge_selected_foat_arcs_window()));
        connect(_side_widget->join_selected_arcs_button, SIGNAL(clicked()), _gl_widget, SLOT(show_join_selected_foat_arcs_window()));
        connect(_side_widget->continue_selected_arc_button, SIGNAL(clicked()), _gl_widget, SLOT(show_continue_selected_foat_arcs_window()));
        connect(_side_widget->translate_arc_along_x_spinbox, SIGNAL(valueChanged(double)), _gl_widget, SLOT(translate_foat_arc_along_x(double)));
        connect(_side_widget->translate_arc_along_y_spinbox, SIGNAL(valueChanged(double)), _gl_widget, SLOT(translate_foat_arc_along_y(double)));
        connect(_side_widget->translate_arc_along_z_spinbox, SIGNAL(valueChanged(double)), _gl_widget, SLOT(translate_foat_arc_along_z(double)));
        connect(_merge_window->buttonBox, SIGNAL(accepted()), _gl_widget, SLOT(merge_selected_foat_arcs()));


        // foat patch
        connect(_side_widget->cp_alpha_u_spin_box, SIGNAL(valueChanged(double)), _gl_widget, SLOT(set_u_alpha(double)));
        connect(_side_widget->cp_alpha_v_spin_box, SIGNAL(valueChanged(double)), _gl_widget, SLOT(set_v_alpha(double)));
        connect(_side_widget->cp_row_spin_box, SIGNAL(valueChanged(int)), _gl_widget, SLOT(set_selected_row_index(int)));
        connect(_side_widget->cp_column_spin_box, SIGNAL(valueChanged(int)), _gl_widget, SLOT(set_selected_column_index(int)));
        connect(_side_widget->cp_x_value_spin_box, SIGNAL(valueChanged(double)), _gl_widget, SLOT(set_selected_point_x_value(double)));
        connect(_side_widget->cp_y_value_spin_box, SIGNAL(valueChanged(double)), _gl_widget, SLOT(set_selected_point_y_value(double)));
        connect(_side_widget->cp_z_value_spin_box, SIGNAL(valueChanged(double)), _gl_widget, SLOT(set_selected_point_z_value(double)));
        connect(_side_widget->insert_patch_button, SIGNAL(clicked(bool)), _gl_widget, SLOT(insert_new_foat_patch()));
        connect(_side_widget->delete_selected_patch_button, SIGNAL(clicked()), _gl_widget, SLOT(delete_selected_foat_patch()));
        connect(_side_widget->cp_selected_patch_spin_box, SIGNAL(valueChanged(int)), _gl_widget, SLOT(set_selected_foat_patch(int)));
        connect(_side_widget->uv_first_deriv_check_box, SIGNAL(clicked(bool)), _gl_widget, SLOT(set_visibility_of_first_order_derivatives(bool)));
        connect(_side_widget->uv_second_deriv_check_box, SIGNAL(clicked(bool)), _gl_widget, SLOT(set_visibility_of_second_order_derivatives(bool)));
        connect(_side_widget->uv_normal_vectors_check_box, SIGNAL(clicked(bool)), _gl_widget, SLOT(set_visibility_of_normal_vectors(bool)));

        connect(_side_widget->u_div_point_count_spin_box, SIGNAL(valueChanged(int)), _gl_widget, SLOT(set_u_div_point_count(int)));
        connect(_side_widget->u_iso_line_count_spin_box, SIGNAL(valueChanged(int)), _gl_widget, SLOT(set_u_iso_line_count(int)));
        connect(_side_widget->v_div_point_count_spin_box, SIGNAL(valueChanged(int)), _gl_widget, SLOT(set_v_div_point_count(int)));
        connect(_side_widget->v_iso_line_count_spin_box, SIGNAL(valueChanged(int)), _gl_widget, SLOT(set_v_iso_line_count(int)));

        connect(_side_widget->uv_scale_spin_box, SIGNAL(valueChanged(double)), _gl_widget, SLOT(set_uv_scale(double)));
        connect(_side_widget->uv_scale_spin_box, SIGNAL(valueChanged(double)), _gl_widget, SLOT(set_normal_scale(double)));
        connect(_side_widget->translate_patch_along_x_spinbox, SIGNAL(valueChanged(double)), _gl_widget, SLOT(translate_foat_patch_along_x(double)));
        connect(_side_widget->translate_patch_along_y_spinbox, SIGNAL(valueChanged(double)), _gl_widget, SLOT(translate_foat_patch_along_y(double)));
        connect(_side_widget->translate_patch_along_z_spinbox, SIGNAL(valueChanged(double)), _gl_widget, SLOT(translate_foat_patch_along_z(double)));

        // FOAT_JOIN
        connect(_join_window->buttonBox, SIGNAL(accepted()), _gl_widget, SLOT(join_arcs()));
        connect(_join_window->foat_join_index_0_spinbox, SIGNAL(valueChanged(int)), _gl_widget, SLOT(set_foat_join_index_0(int)));
        connect(_join_window->foat_join_index_1_spinbox, SIGNAL(valueChanged(int)), _gl_widget, SLOT(set_foat_join_index_1(int)));
        connect(_join_window->foat_join_direction_0_comboBox, SIGNAL(currentIndexChanged(int)), _gl_widget, SLOT(set_foat_join_direction_0(int)));
        connect(_join_window->foat_join_direction_1_comboBox, SIGNAL(currentIndexChanged(int)), _gl_widget, SLOT(set_foat_join_direction_1(int)));

        connect(_join_patch_window->buttonBox, SIGNAL(accepted()), _gl_widget, SLOT(join_patches()));
        connect(_join_patch_window->foat_join_patches_index_0_spinbox, SIGNAL(valueChanged(int)), _gl_widget, SLOT(set_foat_join_patches_index_0(int)));
        connect(_join_patch_window->foat_join_patches_index_1_spinbox, SIGNAL(valueChanged(int)), _gl_widget, SLOT(set_foat_join_patches_index_1(int)));
        connect(_join_patch_window->foat_join_patches_direction_0_comboBox, SIGNAL(currentIndexChanged(int)), _gl_widget, SLOT(set_foat_join_patches_direction_0(int)));
        connect(_join_patch_window->foat_join_patches_direction_1_comboBox, SIGNAL(currentIndexChanged(int)), _gl_widget, SLOT(set_foat_join_patches_direction_1(int)));

        connect(_side_widget->join_patch_button, SIGNAL(clicked()), _gl_widget, SLOT(show_join_selected_foat_patches_window()));


        connect(_side_widget->merge_patch_button, SIGNAL(clicked()), _gl_widget, SLOT(show_merge_patches_window()));
        connect(_merge_patch_window->buttonBox, SIGNAL(accepted()), _gl_widget, SLOT(merge_selected_patches()));

        connect(_continue_window->buttonBox, SIGNAL(accepted()), _gl_widget, SLOT(continue_selected_foat_arc()));
        connect(_side_widget->continue_patch_button, SIGNAL(clicked()), _gl_widget, SLOT(show_continue_patch_window()));
        connect(_continue_patch_window->buttonBox, SIGNAL(accepted()), _gl_widget, SLOT(continue_selected_foat_patch()));


        connect(_gl_widget, SIGNAL(selected_arc_changed(int)), _side_widget->selected_foat_arc_spinBox, SLOT(setValue(int)));
        connect(_gl_widget, SIGNAL(selected_patch_cp_x_changed(double)), _side_widget->cp_x_value_spin_box, SLOT(setValue(double)));
        connect(_gl_widget, SIGNAL(selected_patch_cp_y_changed(double)), _side_widget->cp_y_value_spin_box, SLOT(setValue(double)));
        connect(_gl_widget, SIGNAL(selected_patch_cp_z_changed(double)), _side_widget->cp_z_value_spin_box, SLOT(setValue(double)));

        connect(_gl_widget, SIGNAL(selected_arc_cp_x_changed(double)), _side_widget->arc_cp_translate_x_spinbox, SLOT(setValue(double)));
        connect(_gl_widget, SIGNAL(selected_arc_cp_y_changed(double)), _side_widget->arc_cp_translate_y_spinbox, SLOT(setValue(double)));
        connect(_gl_widget, SIGNAL(selected_arc_cp_z_changed(double)), _side_widget->arc_cp_translate_z_spinbox, SLOT(setValue(double)));

        connect(_side_widget->texture_nr_spin_box, SIGNAL(valueChanged(int)), _gl_widget, SLOT(set_texture_nr(int)));
        connect(_side_widget->material_nr_spin_box, SIGNAL(valueChanged(int)), _gl_widget, SLOT(set_material_nr(int)));
        connect(_side_widget->texture_show_check_box, SIGNAL(clicked(bool)), _gl_widget, SLOT(set_show_texture(bool)));

        connect(_side_widget->save_button, SIGNAL(clicked()), _gl_widget, SLOT(save()));
        connect(_side_widget->load_button, SIGNAL(clicked()), _gl_widget, SLOT(load()));
        connect(_side_widget->tab_widget, SIGNAL(currentChanged(int)), _gl_widget, SLOT(set_render_type(int)));
    }

    //--------------------------------
    // implementation of private slots
    //--------------------------------
    void MainWindow::on_action_Quit_triggered()
    {
        qApp->exit(0);
    }
}
