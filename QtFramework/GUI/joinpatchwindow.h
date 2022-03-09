#ifndef JOINPATCHWINDOW_H
#define JOINPATCHWINDOW_H

#include <QDialog>
#include "ui_joinpatchwindow.h"

namespace cagd {
    class JoinPatchWindow : public QDialog, public Ui::JoinPatchWindow
    {
        Q_OBJECT

    public:
        JoinPatchWindow(QWidget *parent = 0);

    private slots:

    };
}

#endif // JOINPATCHWINDOW_H
