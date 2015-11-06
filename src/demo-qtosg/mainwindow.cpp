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

    /// 3d model and mouse - stylus navigation
    menu->addAction("Create cow scene single view", this, SLOT(onCreateView1()));
    menu->addAction("Create cow scene double view", this, SLOT(onCreateView2()));

    /// empty window / scene with coordinat axes in the corner
    menu->addAction("Create empty scene single view", this, SLOT(onCreateEmptyScene1()));

    this->setCentralWidget(_mdiArea);
}

MainWindow::~MainWindow(){}

void MainWindow::onCreateView2(){
    OSGWidget* osgwid = new OSGWidget(this, 2, "../demo-osg/cow.osgt");
    QObject::connect(this, SIGNAL(sendTabletDevice(bool)),
                     osgwid, SLOT(getTabletDevice(bool)));
    QMdiSubWindow* subwin = _mdiArea->addSubWindow(osgwid);
    subwin->show();
}

void MainWindow::onCreateView1(){
    OSGWidget* osgwid = new OSGWidget(this, 1, "../demo-osg/cow.osgt");
    QObject::connect(this, SIGNAL(sendTabletDevice(bool)),
                     osgwid, SLOT(getTabletDevice(bool)));
    QMdiSubWindow* subwin = _mdiArea->addSubWindow(osgwid);
    subwin->show();
}

void MainWindow::getTabletDevice(bool active){
    _deviceActive = active;
    emit sendTabletDevice(active);
}

void MainWindow::onCreateEmptyScene2(){
    OSGWidget* osgwid = new OSGWidget(this, 2);
    QObject::connect(this, SIGNAL(sendTabletDevice(bool)),
                     osgwid, SLOT(getTabletDevice(bool)));
    QMdiSubWindow* subwin = _mdiArea->addSubWindow(osgwid);
    subwin->show();
}

void MainWindow::onCreateEmptyScene1(){
    OSGWidget* osgwid = new OSGWidget(this, 1);
    QObject::connect(this, SIGNAL(sendTabletDevice(bool)),
                     osgwid, SLOT(getTabletDevice(bool)));
    QMdiSubWindow* subwin = _mdiArea->addSubWindow(osgwid);
    subwin->show();
}
