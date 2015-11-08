#include "mainwindow.h"
#include "viewwidget.h"

#include <QMdiSubWindow>
#include <QMenuBar>
#include <QObject>

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
    _axes(new osg::Geode)
{
    QMenuBar* menuBar = this->menuBar();
    QMenu* menu = menuBar->addMenu("Test");

    menu->addAction("Add scene viewer", this, SLOT(onCreateViewer()));

    this->setCentralWidget(_mdiArea);

    /* Scene graph initializations */
    _axes->addDrawable(createAxes(osg::Vec3(0.0f,0.0f,0.0f), osg::Vec3(1.0f,0.0f,0.0f),
                                  osg::Vec3(0.0f,1.0f,0.0f), osg::Vec3(0.0f,0.0f,1.0f)));
    _root->addChild(_axes.get());
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

osg::Drawable* MainWindow::createAxes(const osg::Vec3& corner,const osg::Vec3& xdir,const osg::Vec3& ydir,const osg::Vec3& zdir)
{
    osg::ref_ptr<osg::Geometry> geom = new osg::Geometry;

    osg::Vec3Array* coords = new osg::Vec3Array(6);
    (*coords)[0] = corner;
    (*coords)[1] = corner+xdir;
    (*coords)[2] = corner;
    (*coords)[3] = corner+ydir;
    (*coords)[4] = corner;
    (*coords)[5] = corner+zdir;

    geom->setVertexArray(coords);

    osg::Vec4 x_color(float(38)/255.0f,float(139)/255.0f,float(210)/255.0f,1.0f); // solarized blue
    osg::Vec4 y_color(float(133)/255.0f,float(153)/255.0f,float(0)/255.0f,1.0f); // solarized green
    osg::Vec4 z_color(float(211)/255.0f,float(54)/255.0f,float(130)/255.0f,1.0f); // solarized magenta

    osg::Vec4Array* color = new osg::Vec4Array(6);
    (*color)[0] = x_color;
    (*color)[1] = x_color;
    (*color)[2] = y_color;
    (*color)[3] = y_color;
    (*color)[4] = z_color;
    (*color)[5] = z_color;

    geom->setColorArray(color, osg::Array::BIND_PER_VERTEX);

    geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINES,0,6));

    osg::StateSet* stateset = new osg::StateSet;
    osg::LineWidth* linewidth = new osg::LineWidth();
    linewidth->setWidth(4.0f);
    stateset->setAttributeAndModes(linewidth,osg::StateAttribute::ON);
    stateset->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
    geom->setStateSet(stateset);

    return geom.release();
}
