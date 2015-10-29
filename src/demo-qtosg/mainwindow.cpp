#include "mainwindow.h"
#include "osgwidget.h"

#include <QDebug>
#include <QMdiSubWindow>
#include <QMenuBar>

MainWindow::MainWindow(QWidget *parent, Qt::WindowFlags flags) :
    QMainWindow(parent, flags),
    _mdiArea(new QMdiArea(this))
{
    QMenuBar* menuBar = this->menuBar();
    QMenu* menu = menuBar->addMenu("Test");
    menu->addAction("Create view", this, SLOT(onCreateView()));

    this->setCentralWidget(_mdiArea);
}

MainWindow::~MainWindow(){}

void MainWindow::onCreateView(){
    OSGWidget* osgwid = new OSGWidget(this);
    QMdiSubWindow* subwin = _mdiArea->addSubWindow(osgwid);
    subwin->show();
}
