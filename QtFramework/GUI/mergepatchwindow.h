#ifndef MERGEPATCHWINDOW_H
#define MERGEPATCHWINDOW_H

#include <QDialog>
#include "ui_mergepatchwindow.h"

namespace cagd {
    class MergePatchWindow : public QDialog, public Ui::MergePatchWindow
    {
        Q_OBJECT

    public:
        MergePatchWindow(QWidget *parent = 0);
    };
}


#endif // MERGEPATCHWINDOW_H
