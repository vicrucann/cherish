#include "mainwindow.h"
#include "osgwidget.h"

#include <QDebug>
#include <QMdiSubWindow>

MainWindow::MainWindow(QWidget *parent, Qt::WindowFlags flags) :
    QMainWindow(parent, flags),
    _mdiArea(new QMdiArea(this))
{
    this->setCentralWidget(_mdiArea);

    OSGWidget* osgwid = new OSGWidget(this);
    QMdiSubWindow* subwin = _mdiArea->addSubWindow(osgwid);
    subwin->show();
}

MainWindow::~MainWindow(){}
