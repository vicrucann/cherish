#include "mainwindow.h"
#include "viewwidget.h"
#include "canvas.h"
#include "settings.h"

#include <QMdiSubWindow>
#include <QMenuBar>
#include <QObject>

#include <osg/Group>
#include <osg/StateSet>
#include <osg/Material>
#include <osg/Drawable>
#include <osg/LineWidth>

osg::Node* createCanvas(osg::BoundingBox& bb){
    osg::Vec3 top_left(bb.xMin(),bb.yMin(),bb.zMax());
    osg::Vec3 bottom_left(bb.xMin(),bb.yMin(),bb.zMin());
    osg::Vec3 bottom_right(bb.xMax(),bb.yMin(),bb.zMin());
    osg::Vec3 top_right(bb.xMax(),bb.yMin(),bb.zMax());

    osg::Geometry* geom = new osg::Geometry;
    osg::Vec3Array* vertices = new osg::Vec3Array(4);
    (*vertices)[0] = top_left;
    (*vertices)[1] = bottom_left;
    (*vertices)[2] = bottom_right;
    (*vertices)[3] = top_right;
    geom->setVertexArray(vertices);

    osg::Vec4 border_clr(float(101)/255.0f,float(123)/255.0f,float(131)/255.0f,1.0f); // solarized base00
    osg::Vec4Array* colors = new osg::Vec4Array(4);
    (*colors)[0] = border_clr;
    (*colors)[1] = border_clr;
    (*colors)[2] = border_clr;
    (*colors)[3] = border_clr;
    geom->setColorArray(colors, osg::Array::BIND_OVERALL);
    geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINE_LOOP,0,4));

    osg::Geode* geode = new osg::Geode;
    geode->addDrawable(geom);

    return geode;
}

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

    /* Scene graph minimal initializations */
    _axes->addDrawable(createAxes(osg::Vec3(0.0f,0.0f,0.0f), osg::Vec3(0.1f,0.0f,0.0f),
                                  osg::Vec3(0.0f,0.1f,0.0f), osg::Vec3(0.0f,0.0f,0.1f)));
    _root->addChild(_axes.get());

    osg::BoundingBox bb(0.0f,0.0f,0.0f,1.0f,1.0f,1.0f);
    _root->addChild(createCanvas(bb));

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

    //osg::Vec4 x_color(float(38)/255.0f,float(139)/255.0f,float(210)/255.0f,1.0f); // solarized blue
    //osg::Vec4 y_color(float(133)/255.0f,float(153)/255.0f,float(0)/255.0f,1.0f); // solarized green
    //osg::Vec4 z_color(float(211)/255.0f,float(54)/255.0f,float(130)/255.0f,1.0f); // solarized magenta

    osg::Vec4Array* color = new osg::Vec4Array(6);
    (*color)[0] = dureu::AXES_CLR_X;
    (*color)[1] = dureu::AXES_CLR_X;
    (*color)[2] = dureu::AXES_CLR_Y;
    (*color)[3] = dureu::AXES_CLR_Y;
    (*color)[4] = dureu::AXES_CLR_Z;
    (*color)[5] = dureu::AXES_CLR_Z;

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
