#include "joinwindow.h"
#include "ui_joinwindow.h"
#include <iostream>

namespace cagd {

    JoinWindow::JoinWindow(QWidget *parent) :
        QDialog(parent)
    {
        setupUi(this);
    }
}
