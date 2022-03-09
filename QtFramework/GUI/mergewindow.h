#ifndef MERGEWINDOW_H
#define MERGEWINDOW_H

#include <QDialog>
#include "ui_mergewindow.h"

namespace cagd {
    class MergeWindow : public QDialog, public Ui::MergeWindow
    {
        Q_OBJECT

    public:
        MergeWindow(QWidget *parent = 0);
    };
}

#endif // MERGEWINDOW_H
