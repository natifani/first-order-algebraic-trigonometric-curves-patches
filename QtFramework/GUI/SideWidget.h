#pragma once

#include <QWidget>
#include "ui_SideWidget.h"
#include "mergewindow.h"

namespace cagd
{
    class SideWidget: public QWidget, public Ui::SideWidget
    {
    public:
        // special and default constructor
        SideWidget(QWidget *parent = 0);
    };
}
