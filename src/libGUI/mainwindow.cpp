#include <iostream>

#include <QMdiSubWindow>
#include <QMenuBar>
#include <QObject>
#include <QRect>
#include <QSize>

#include "mainwindow.h"
#include "viewwidget.h"
#include "settings.h"
#include "bookmarkwidget.h"

MainWindow::MainWindow(QWidget *parent, Qt::WindowFlags flags) :
    QMainWindow(parent, flags),
    _desktop(0),
    _mdiArea(new QMdiArea(this)),
    _bookmarks(new BookmarkWidget(this)),
    _tabletActive(false),
    _rootScene(new RootScene())
{
    this->onCreateViewer();
    //this->addDockWidget(Qt::LeftDockWidgetArea, _bookmarks);

    QMenuBar* menuBar = this->menuBar();
    QMenu* menuTest = menuBar->addMenu("Test");

    menuTest->addAction("Add scene double viewer", this, SLOT(onCreateDoubleViewer()));
    menuTest->addAction("Add outisde viewer", this, SLOT(onCreateOutsideViewer()));
    menuTest->addAction("Load cow to the scene",  this, SLOT(onLoadCow()));
    menuTest->addAction("Stylus draw ON", this, SLOT(onSetStylusSketchON()));
    menuTest->addAction("Stylus draw OFF", this, SLOT(onSetStylusSketchOFF()));
    menuTest->addAction("Global axes ON", this, SLOT(onSetGloAxesON()));
    menuTest->addAction("Global axes OFF", this, SLOT(onSetGloAxesOFF()));

    QMenu* menuTC = menuBar->addMenu("TestModels");
    menuTC->addAction("Delete canvas 3", this, SLOT(onDeleteCanvas()));
    menuTC->addAction("Delete cow.osg", this, SLOT(onDeleteCow()));

    QMenu* menuTM = menuBar->addMenu("TestMouse");
    menuTM->addAction("Naviagation: rotate", this, SLOT(onMouseRotate()));
    menuTM->addAction("Navigation: zoom", this, SLOT(onMouseZoom()));
    menuTM->addAction("Naviagation: pan", this, SLOT(onMousePan()));
    menuTM->addAction("Mouse: pick", this, SLOT(onMousePick()));

    this->setCentralWidget(_mdiArea);
}

MainWindow::~MainWindow(){}

void MainWindow::SetDesktopWidget(QDesktopWidget *desktop, dureu::APPMODE mode) {
    _desktop = desktop;
    QRect availS = _desktop->availableGeometry();
    QRect fullS = _desktop->geometry();
    int nscreen = _desktop->screenCount();
    double scale = 0.9;
    double scale_inv = 1-scale;
    switch (mode) {
    case dureu::SCREEN_MIN:
        this->showNormal();
        break;
    case dureu::SCREEN_MAX:
        this->showMaximized();
        break;
    case dureu::SCREEN_FULL:
        this->showFullScreen();
        break;
    case dureu::SCREEN_VIRTUAL: // needs testing and fixing
        this->resize(QSize(fullS.width(), fullS.height()));
        break;
    case dureu::SCREEN_DETACHED:
        this->resize(QSize(availS.width()*scale, fullS.height()*scale_inv));
        this->move(availS.width()*scale_inv, fullS.height()-availS.height());
        break;
    default:
        std::cerr << "Application mode not recognized, closing application" << std::endl;
        exit(1);
    }
}

void MainWindow::getTabletActivity(bool active){
    _tabletActive = active;
    emit sendTabletActivity(active);
}

/* Create an ordinary single view window on the scene _root */
void MainWindow::onCreateViewer(){    
    ViewWidget* vwid = createViewer();
    QMdiSubWindow* subwin = _mdiArea->addSubWindow(vwid);
    subwin->setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    vwid->showMaximized();
    subwin->show();
}

void MainWindow::onCreateDoubleViewer(){
    ViewWidget* vwid = createViewer(Qt::Widget, 2);
    QMdiSubWindow* subwin = _mdiArea->addSubWindow(vwid);
    subwin->show();
}

void MainWindow::onCreateOutsideViewer(){
    ViewWidget* vwid = createViewer(Qt::Window);
    vwid->show();
}

void MainWindow::onLoadCow(){
    bool success = _rootScene->loadSceneFromFile("../../samples/cow.osgt", "cow.node");
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

void MainWindow::onSetGloAxesON() {
    _rootScene->setAxesVisibility(true);
}

void MainWindow::onSetGloAxesOFF() {
    _rootScene->setAxesVisibility(false);
}

void MainWindow::onDeleteCanvas()
{
    // this is just to show how a certain canvas can be deleted
    _rootScene->deleteCanvas(2); // we delete a canvas with id "3"
}

void MainWindow::onDeleteCow() {
    _rootScene->deleteNode("cow.node");
}

void MainWindow::onMouseRotate()
{
}

void MainWindow::onMouseZoom()
{

}

void MainWindow::onMousePan()
{

}

void MainWindow::onMousePick()
{

}

ViewWidget* MainWindow::createViewer(Qt::WindowFlags f, int viewmode)
{
    ViewWidget* vwid = new ViewWidget(_rootScene, this, f, viewmode);
    QObject::connect(this, SIGNAL(sendTabletActivity(bool)),
                     vwid, SLOT(getTabletActivity(bool)));
    QObject::connect(this, SIGNAL(sendStylusSketchStatus(bool)),
                     vwid, SLOT(getStylusSketchStatus(bool)) );
    return vwid;
}

