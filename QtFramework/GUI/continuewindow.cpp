#include "continuewindow.h"
#include "ui_continuewindow.h"
#include <iostream>

namespace cagd {

    ContinueWindow::ContinueWindow(QWidget *parent) :
        QDialog(parent)
    {
        setupUi(this);
    }
}

