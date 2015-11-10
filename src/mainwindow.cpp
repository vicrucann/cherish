#include "mainwindow.h"
#include "viewwidget.h"
#include "canvas.h"
#include "axes.h"
#include "settings.h"

#include <QMdiSubWindow>
#include <QMenuBar>
#include <QObject>

#include <osg/ref_ptr>
#include <osg/Group>
#include <osg/StateSet>
#include <osg/Material>
#include <osg/Drawable>
#include <osg/LineWidth>

MainWindow::MainWindow(QWidget *parent, Qt::WindowFlags flags) :
    QMainWindow(parent, flags),
    _mdiArea(new QMdiArea(this)),
    _tabletActive(false),
    _root(new osg::Group),
    _scene(new osg::Group)
{
    QMenuBar* menuBar = this->menuBar();
    QMenu* menu = menuBar->addMenu("Test");

    menu->addAction("Add scene viewer", this, SLOT(onCreateViewer()));

    this->setCentralWidget(_mdiArea);

    /* Scene graph minimal initializations */
    osg::ref_ptr<Axes> axes = new Axes(osg::Vec3(0.0f,0.0f,0.0f), osg::Vec3(dureu::AXES_SIZE,0.0f,0.0f),
                                       osg::Vec3(0.0f,dureu::AXES_SIZE,0.0f), osg::Vec3(0.0f,0.0f,dureu::AXES_SIZE));
    _root->addChild(axes.get());

    osg::ref_ptr<Canvas> cnv_xy = new Canvas(osg::Vec3(1.0f,1.0f,0.0f),
                                          osg::Vec3(dureu::CANVAS_MINW, dureu::CANVAS_MINH, 0.0f),
                                          osg::Vec3(-dureu::CANVAS_MINW, dureu::CANVAS_MINH,0.0f),
                                          dureu::CANVAS_CLR_PREVIOUS);
    osg::ref_ptr<Canvas> cnv_xz = new Canvas(osg::Vec3(1.0f,0.0f,1.0f),
                                          osg::Vec3(dureu::CANVAS_MINW, 0.0f, dureu::CANVAS_MINH),
                                          osg::Vec3(-dureu::CANVAS_MINW,0.0f, dureu::CANVAS_MINH),
                                          dureu::CANVAS_CLR_CURRENT);
    osg::ref_ptr<Canvas> cnv_yz = new Canvas(osg::Vec3(0.0f,1.0f,1.0f),
                                          osg::Vec3(0.0f, dureu::CANVAS_MINW, dureu::CANVAS_MINH),
                                          osg::Vec3(0.0f,-dureu::CANVAS_MINW, dureu::CANVAS_MINH),
                                          dureu::CANVAS_CLR_REST);

    _scene->addChild(cnv_xy.get());
    _scene->addChild(cnv_xz.get());
    _scene->addChild(cnv_yz.get());
    _root->addChild(_scene.get());
}

MainWindow::~MainWindow(){}

void MainWindow::getTabletActivity(bool active){
    _tabletActive = active;
    emit sendTabletActivity(active);
}

/* Create an ordinary single view window on the scene _root */
void MainWindow::onCreateViewer(){    
    ViewWidget* vwid = new ViewWidget(_root, this);
    QObject::connect(this, SIGNAL(sendTabletActivity(bool)),
                     vwid, SLOT(getTabletActivity(bool)));
    QMdiSubWindow* subwin = _mdiArea->addSubWindow(vwid);
    subwin->show();
}

