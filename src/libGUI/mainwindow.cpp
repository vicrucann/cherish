#include <QMdiSubWindow>
#include <QMenuBar>
#include <QObject>

#include "mainwindow.h"
#include "viewwidget.h"
#include "settings.h"

MainWindow::MainWindow(QWidget *parent, Qt::WindowFlags flags) :
    QMainWindow(parent, flags),
    _mdiArea(new QMdiArea(this)),
    _tabletActive(false),
    _rootScene(new RootScene())
{
    QMenuBar* menuBar = this->menuBar();
    QMenu* menuTest = menuBar->addMenu("Test");

    menuTest->addAction("Add scene viewer", this, SLOT(onCreateViewer()));
    menuTest->addAction("Add scene double viewer", this, SLOT(onCreateDoubleViewer()));
    menuTest->addAction("Add outisde viewer", this, SLOT(onCreateOutsideViewer()));
    menuTest->addAction("Load cow to the scene",  this, SLOT(onLoadCow()));
    menuTest->addAction("Stylus draw ON", this, SLOT(onSetStylusSketchON()));
    menuTest->addAction("Stylus draw OFF", this, SLOT(onSetStylusSketchOFF()));

    this->setCentralWidget(_mdiArea);
}

MainWindow::~MainWindow(){}

void MainWindow::getTabletActivity(bool active){
    _tabletActive = active;
    emit sendTabletActivity(active);
}

/* Create an ordinary single view window on the scene _root */
void MainWindow::onCreateViewer(){    
    ViewWidget* vwid = new ViewWidget(_rootScene, this);
    QObject::connect(this, SIGNAL(sendTabletActivity(bool)),
                     vwid, SLOT(getTabletActivity(bool)));
    QObject::connect(this, SIGNAL(sendStylusSketchStatus(bool)),
                     vwid, SLOT(getStylusSketchStatus(bool)) );
    QMdiSubWindow* subwin = _mdiArea->addSubWindow(vwid);
    subwin->show();
}

void MainWindow::onCreateDoubleViewer(){
    ViewWidget* vwid = new ViewWidget(_rootScene, this, Qt::Widget, 2);
    QObject::connect(this, SIGNAL(sendTabletActivity(bool)),
                     vwid, SLOT(getTabletActivity(bool)));
    QObject::connect(this, SIGNAL(sendStylusSketchStatus(bool)),
                     vwid, SLOT(getStylusSketchStatus(bool)) );
    QMdiSubWindow* subwin = _mdiArea->addSubWindow(vwid);
    subwin->show();
}

void MainWindow::onCreateOutsideViewer(){
    ViewWidget* vwid = new ViewWidget(_rootScene, this, Qt::Window);
    QObject::connect(this, SIGNAL(sendTabletActivity(bool)),
                     vwid, SLOT(getTabletActivity(bool)));
    QObject::connect(this, SIGNAL(sendStylusSketchStatus(bool)),
                     vwid, SLOT(getStylusSketchStatus(bool)) );
    vwid->show();
}

void MainWindow::onLoadCow(){
    bool success = _rootScene->loadSceneFromFile("../../samples/cow.osgt");
    if (!success){
        std::cerr << "The filename provided was not correct, or the model could not be read" << std::endl;
    }
}

void MainWindow::onSetStylusSketchON(){
    emit sendStylusSketchStatus(true);
}

void MainWindow::onSetStylusSketchOFF(){
    emit sendStylusSketchStatus(false);
}

