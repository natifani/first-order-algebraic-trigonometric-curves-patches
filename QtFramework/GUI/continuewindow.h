#ifndef CONTINUEWINDOW_H
#define CONTINUEWINDOW_H

#include <QDialog>
#include "ui_continuewindow.h"

namespace cagd {
    class ContinueWindow : public QDialog, public Ui::ContinueWindow
    {
        Q_OBJECT

    public:
        ContinueWindow(QWidget *parent = 0);

    private slots:

    };
}
#endif // CONTINUEWINDOW_H
