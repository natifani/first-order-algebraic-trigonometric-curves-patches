#include "mergewindow.h"
#include "ui_mergewindow.h"
#include <iostream>

namespace cagd {

    MergeWindow::MergeWindow(QWidget *parent) :
        QDialog(parent)
    {
        setupUi(this);
    }
}
