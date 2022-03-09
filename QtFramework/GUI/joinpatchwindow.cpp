#include "joinpatchwindow.h"
#include "ui_joinpatchwindow.h"
#include <iostream>

namespace cagd {

    JoinPatchWindow::JoinPatchWindow(QWidget *parent) :
        QDialog(parent)
    {
        setupUi(this);
    }
}
