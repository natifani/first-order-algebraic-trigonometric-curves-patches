#pragma once

#include <QMainWindow>
#include "ui_MainWindow.h"
#include "SideWidget.h"
#include "GLWidget.h"
#include <QScrollArea>
#include <GUI/mergepatchwindow.h>

namespace cagd
{
    class MainWindow: public QMainWindow, public Ui::MainWindow
    {
        Q_OBJECT

    private:
        SideWidget  *_side_widget;      // pointer to our side widget
        GLWidget    *_gl_widget;        // pointer to our rendering OpenGL widget

        QScrollArea *_scroll_area;      // provides a scrolling view onto our side widget

        MergeWindow *_merge_window; //foat
        ContinueWindow *_continue_window; //foat
        JoinWindow *_join_window; //foat
        JoinPatchWindow *_join_patch_window;
        MergePatchWindow *_merge_patch_window;
        ContinuePatchWindow * _continue_patch_window;

    public:
        // special and default constructor
        MainWindow(QWidget *parent = 0);

    private slots:
        // private event handling methods/slots
        void on_action_Quit_triggered();
    };
}
