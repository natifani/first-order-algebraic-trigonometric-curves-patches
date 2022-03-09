#ifndef JOINWINDOW_H
#define JOINWINDOW_H

#include <QDialog>
#include "ui_joinwindow.h"

namespace cagd {
    class JoinWindow : public QDialog, public Ui::JoinWindow
    {
        Q_OBJECT

    public:
        JoinWindow(QWidget *parent = 0);

    private slots:

    };
}
#endif // JOINWINDOW_H
