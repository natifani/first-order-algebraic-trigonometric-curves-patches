#ifndef CONTINUEPATCHWINDOW_H
#define CONTINUEPATCHWINDOW_H

#include <QDialog>
#include "ui_continuepatchwindow.h"

namespace cagd {
    class ContinuePatchWindow : public QDialog, public Ui::ContinuePatchWindow
    {
        Q_OBJECT

    public:
        ContinuePatchWindow(QWidget *parent = 0);

    private slots:

    };
}
#endif // CONTINUEPATCHWINDOW_H
