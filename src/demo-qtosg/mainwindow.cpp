#include <iostream>
#include "mainwindow.h"
#include "osgwidget.h"

#include <QDebug>
#include <QMdiSubWindow>
#include <QMenuBar>
#include <QObject>

MainWindow::MainWindow(QWidget *parent, Qt::WindowFlags flags) :
    QMainWindow(parent, flags),
    _mdiArea(new QMdiArea(this)),
    _deviceActive(false)
{
    QMenuBar* menuBar = this->menuBar();
    QMenu* menu = menuBar->addMenu("Test");
    menu->addAction("Create view", this, SLOT(onCreateView()));

    this->setCentralWidget(_mdiArea);
}

MainWindow::~MainWindow(){}

void MainWindow::onCreateView(){
    OSGWidget* osgwid = new OSGWidget(this);
    QObject::connect(this, SIGNAL(sendTabletDevice(bool)),
                     osgwid, SLOT(getTabletDevice(bool)));
    QMdiSubWindow* subwin = _mdiArea->addSubWindow(osgwid);
    subwin->show();
}

void MainWindow::getTabletDevice(bool active){
    _deviceActive = active;
    std::cout << "MainWindow active: " << active << std::endl;
    emit sendTabletDevice(active);
}
