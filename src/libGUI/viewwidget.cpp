#include <cassert>
#include <vector>
#include <iostream>
#include <stdexcept>
#include <math.h>

#include <QDebug>
#include <QKeyEvent>
#include <QPainter>
#include <QWheelEvent>

#include <osg/Camera>
#include <osg/StateSet>
#include <osg/Material>
#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>
#include <osgGA/EventQueue>
#include <osgGA/TrackballManipulator>
#include <osg/Matrix>

#include "viewwidget.h"
#include "settings.h"
#include "eventhandler.h"
#include "basehandler.h"

ViewWidget::ViewWidget(osg::ref_ptr<RootScene> &root, QWidget *parent, Qt::WindowFlags f, int viewmode):
    QOpenGLWidget(parent, f),
    _graphicsWindow(new osgViewer::GraphicsWindowEmbedded(this->x(), this->y(), this->width(),this->height()) ),
    _viewer(new osgViewer::CompositeViewer),
    _root(root),
    _tabletDevice(QTabletEvent::Stylus), // http://doc.qt.io/qt-5/qtabletevent.html#TabletDevice-enum
    _viewmode(viewmode),
    _deviceDown(false),
    _deviceActive(false),
    _deviceSketch(false)
{
    osg::StateSet* stateSet = _root->getOrCreateStateSet();
    osg::Material* material = new osg::Material;
    material->setColorMode(osg::Material::AMBIENT_AND_DIFFUSE);
    stateSet->setAttributeAndModes(material, osg::StateAttribute::ON);
    stateSet->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);

    float aspectRatio = static_cast<float>( this->width() /_viewmode ) / static_cast<float>( this->height() );
    osg::Camera* camera = new osg::Camera;
    camera->setViewport( 0, 0, this->width() / _viewmode, this->height());
    camera->setProjectionMatrixAsPerspective( 30.f, aspectRatio, 1.f, 1000.f );
    camera->setGraphicsContext( _graphicsWindow );
    camera->setClearColor(dureu::BACKGROUND_CLR);

    // if manipulator is disabled, make sure to take care of view matrix
    // and then you can process event by evenhandler.
    //osgGA::TrackballManipulator* manipulator = new osgGA::TrackballManipulator;
    //manipulator->setAllowThrow( false );

    osgViewer::View* view = new osgViewer::View;
    view->setName("Single view");
    view->setCamera(camera);
    view->setSceneData(_root.get());
    view->addEventHandler(new BaseHandler);
    //view->addEventHandler(new osgViewer::StatsHandler);
    //view->addEventHandler(new PickHandler);
    //view->setCameraManipulator(manipulator);

    _viewer->addView(view);

    if (_viewmode == 2) {
        osgViewer::View* sideView = new osgViewer::View;
        sideView->getCamera()->setGraphicsContext(_graphicsWindow);
        sideView->getCamera()->setViewport(this->width()/_viewmode, 0, this->width()/_viewmode, this->height());
        sideView->getCamera()->setClearColor(dureu::BACKGROUND_CLR);
        sideView->setSceneData( _root.get() );

        osg::Vec3 lookDir = osg::Vec3(-osg::X_AXIS);
        osg::Vec3 up = osg::Vec3(osg::Z_AXIS);
        osg::Vec3 center = _root->getBound().center();
        double radius = _root->getBound().radius();
        sideView->getCamera()->setViewMatrixAsLookAt(center-lookDir*(radius*3.f),
                                                     center,
                                                     up);
        sideView->getCamera()->setProjectionMatrixAsPerspective(30.f, aspectRatio, 1.f, 1000.f);
        sideView->addEventHandler(new EventHandler(dureu::MOUSE_NAVIGATE_FIXED));

        _viewer->addView(sideView);
    }

    _viewer->setThreadingModel(osgViewer::CompositeViewer::SingleThreaded);
    _viewer->realize();
    int minS = 512;
    if (parent->width() > parent->height())
        this->setMinimumSize(minS*_viewmode, minS);
    else
        this->setMinimumSize(minS, minS*_viewmode);
    this->setFocusPolicy(Qt::StrongFocus);
    this->setMouseTracking(true);
}

ViewWidget::~ViewWidget(){}

void ViewWidget::getTabletActivity(bool active){
    _deviceActive = active;
}

void ViewWidget::getStylusSketchStatus(bool sketch){
    _deviceSketch = sketch;
}

void ViewWidget::paintEvent(QPaintEvent *pev){
    this->makeCurrent();
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    this->paintGL();
    //if (_selectionActive && !_selectionFinished){
    //    painter.setPen( Qt::black );
    //    painter.setBrush( Qt::transparent );
    //    painter.drawRect(makeRectangle(_selectionStart, _selectionEnd));
    //}
    painter.end();
    this->doneCurrent();
}

void ViewWidget::paintGL(){
    _viewer->frame();
}

void ViewWidget::resizeGL(int w, int h){
    this->getEventQueue()->windowResize(this->x(), this->y(), w, h);
    _graphicsWindow->resized(this->x(), this->y(), w, h);
    this->onResize(w, h);
}

void ViewWidget::keyPressEvent(QKeyEvent *event){
    QString keystr = event->text();
    const char* keydat = keystr.toLocal8Bit().data();
    switch (event->key()) {
    case Qt::Key_H:
        this->onHome();
        return;
    default:
        break;
    }
    this->getEventQueue()->keyPress(osgGA::GUIEventAdapter::KeySymbol (*keydat));
}

void ViewWidget::keyReleaseEvent(QKeyEvent *event){
    QString keystr = event->text();
    const char* keydat = keystr.toLocal8Bit().data();
    this->getEventQueue()->keyRelease(osgGA::GUIEventAdapter::KeySymbol(*keydat));
}

void ViewWidget::mouseMoveEvent(QMouseEvent *event){
    if (!_deviceDown || !_deviceActive) {
        this->getEventQueue()->mouseMotion(static_cast<float>(event->x()), static_cast<float>(event->y()));
    }
}

void ViewWidget::mousePressEvent( QMouseEvent* event )
{
    if (!_deviceDown || !_deviceActive) {
        unsigned int button = 0;
        switch( event->button() ) {
        case Qt::LeftButton:
            button = 1;
            break;
        case Qt::MiddleButton:
            button = 2;
            break;
        case Qt::RightButton:
            button = 3;
            break;
        default:
            break;
        }
        this->getEventQueue()->mouseButtonPress(static_cast<float>(event->x()), static_cast<float>(event->y()), button);
    }
}

void ViewWidget::mouseReleaseEvent(QMouseEvent* event) {
    if (!_deviceDown || !_deviceActive) {
        unsigned int button = 0;
        switch( event->button() ) {
        case Qt::LeftButton:
            button = 1;
            break;
        case Qt::MiddleButton:
            button = 2;
            break;
        case Qt::RightButton:
            button = 3;
            break;
        default:
            break;
        }
        this->getEventQueue()->mouseButtonRelease(static_cast<float>(event->x()), static_cast<float>(event->y()), button);
    }
}

void ViewWidget::wheelEvent( QWheelEvent* event )
{
    event->accept();
    int delta = event->delta();
    osgGA::GUIEventAdapter::ScrollingMotion motion = delta > 0 ?   osgGA::GUIEventAdapter::SCROLL_UP
                                                                 : osgGA::GUIEventAdapter::SCROLL_DOWN;
    this->getEventQueue()->mouseScroll( motion );
}

void ViewWidget::tabletEvent(QTabletEvent *event){
    unsigned int button = 0;
    int osg_rotate = 1, osg_pan = 2, osg_zoom = 3;
    switch (event->type()){
    case QEvent::TabletPress:
        switch(event->buttons()) {
        /// buttons() define the Wacom behaviour
        /// 1: touch and no buttons are pressed, default: rotate
        /// 2: no touch and lower button pressed, default: zoom
        /// 3: touch and lower button pressed, default: pan
        /// 4: no touch and upper button pressed, default: rotate
        /// 5: touch and upper button pressed, default: rotate
        case 1:
            if (_deviceSketch && _deviceDown){
                std::cout << "stylus press: start sketch" << std::endl;
                button = 0;
                break;
            }
            else
                break;
        case 2:
            button = osg_rotate;
            break;
        case 3:
            button = osg_rotate;
            break;
        case 4:
            button = osg_pan;
            break;
        case 5:
            button = osg_pan;
            break;
        default:
            break;
        }
        this->getEventQueue()->mouseButtonPress(static_cast<float>(event->x()), static_cast<float>(event->y()), button);
        break;
    case QEvent::TabletRelease:
        switch(event->button()) {
        /// buttons() will not work here, so have to use button()
        /// 1: no button released, whether in the air or in touch
        /// 2: lower button released, whether in the air or in touch
        /// 4: top button released, whether in the air or in touch
        case 1:
            if (_deviceSketch && _deviceDown){
                std::cout << "stylus release: stop sketch" << std::endl;
                button = 0;
                break;
            }
            else
                break;
        case 2:
            button = osg_rotate;
            break;
        case 4:
            button = osg_pan;
            break;
        default:
            break;
        }
        this->getEventQueue()->mouseButtonRelease(static_cast<float>(event->x()), static_cast<float>(event->y()), button);
        break;
    case QEvent::TabletMove:
        if (_deviceSketch && _deviceDown){
            button = 0;
            std::cout << "stylus sketching" << std::endl;
        }
        this->getEventQueue()->mouseMotion(static_cast<float>(event->x()), static_cast<float>(event->y()));
        break;
    }
}

bool ViewWidget::event( QEvent* event ) {
    bool handled = QOpenGLWidget::event( event );
    switch( event->type() ) {
    case QEvent::TabletMove:
        event->accept();
        this->update();
        break;
    case QEvent::TabletPress:
        event->accept();
        _deviceDown = true;
        this->update();
        break;
    case QEvent::TabletRelease:
        event->accept();
        this->update();
        _deviceDown = false;
        break;
    case QEvent::KeyPress:
    case QEvent::KeyRelease:
    case QEvent::MouseButtonDblClick:
    case QEvent::MouseButtonPress:
    case QEvent::MouseButtonRelease:
    case QEvent::MouseMove:
    case QEvent::Wheel:
        this->update();
        break;
    default:
        break;
    }
    return handled;
}

void ViewWidget::onHome() {
    osgViewer::ViewerBase::Views views;
    _viewer->getViews( views );
    for( std::size_t i = 0; i < views.size(); i++ ) {
        osgViewer::View* view = views.at(i);
        view->home();
    }
}

void ViewWidget::onResize( int w, int h ) {
    std::vector<osg::Camera*> cameras;
    _viewer->getCameras( cameras );
    assert( cameras.size() == (unsigned int)_viewmode);
    //cameras[0]->setViewport( 0, 0, this->width(), this->height() );
    cameras[0]->setViewport( 0, 0, this->width() / _viewmode, this->height() );
    if (_viewmode == 2)
        cameras[1]->setViewport( this->width() / _viewmode, 0, this->width() / _viewmode, this->height() );
}

osgGA::EventQueue* ViewWidget::getEventQueue() const {
    osgGA::EventQueue* eventQueue = _graphicsWindow->getEventQueue();
    if( eventQueue )
        return eventQueue;
    else
        throw std::runtime_error( "Unable to obtain valid event queue");
}

