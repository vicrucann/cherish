#include "glwidget.h"

#include <iostream>
#include <stdexcept>
#include <vector>

#include <QKeyEvent>
#include <QWheelEvent>

#include <osg/StateSet>
#include <osg/Material>
#include <osg/Camera>
#include <osgViewer/Viewer>
#include <osgGA/EventQueue>
#include <osgGA/EventQueue>
#include <osgGA/TrackballManipulator>

GLWidget::GLWidget(RootScene *root, QWidget *parent, Qt::WindowFlags f)
    : QOpenGLWidget(parent, f)

    , m_GraphicsWindow(new osgViewer::GraphicsWindowEmbedded(this->x(), this->y(), this->width(), this->height()))
    , m_Viewer(new osgViewer::CompositeViewer)
    , m_RootScene(root)
    , m_TabletDevice(QTabletEvent::Stylus) // http://doc.qt.io/qt-5/qtabletevent.html#TabletDevice-enum

    , m_ModeView(1)
    , m_DeviceDown(false)
    , m_DeviceActive(false)
    , m_DeviceSketch(false)
    , m_ModeMouse(dureu::MOUSE_SKETCH)

    , m_Manipulator(new Manipulator(m_ModeMouse))
    , m_EH(new EventHandler(m_RootScene.get(), m_ModeMouse))
{
    // camera settings
    float ratio = static_cast<float>(this->width()) / static_cast<float>( this->height());
    osg::Vec3 look = osg::Vec3(osg::Y_AXIS);
    osg::Vec3 up = osg::Vec3(osg::Z_AXIS);
    osg::Vec3 center = m_RootScene->getBound().center();
    double radius = m_RootScene->getBound().radius();
    osg::Camera* camera = new osg::Camera;
    camera->setViewport(0,0,this->width(), this->height());
    camera->setProjectionMatrixAsPerspective(30.f, ratio, 1.f, 1000.f);
    camera->setViewMatrixAsLookAt(center-look*(radius*3.f), center, up);
    camera->setGraphicsContext(m_GraphicsWindow.get());
    camera->setClearColor(dureu::BACKGROUND_CLR);

    // view settings
    osgViewer::View* view = new osgViewer::View;
    view->setCamera(camera);
    view->setSceneData(m_RootScene.get());
    view->setCameraManipulator(m_Manipulator.get());
    view->addEventHandler(m_EH.get());

    // manipulator settings
    m_Manipulator->setAllowThrow(false);

    // viewer settings
    m_Viewer->addView(view);
    m_Viewer->setThreadingModel(osgViewer::CompositeViewer::SingleThreaded);
    m_Viewer->realize();

    // widget settings
    this->setFocusPolicy(Qt::StrongFocus);
    this->setMouseTracking(true);
}

GLWidget::~GLWidget()
{
}

void GLWidget::getTabletActivity(bool active)
{
    m_DeviceActive = active;
}

void GLWidget::getStylusSketchStatus(bool sketch)
{
    m_DeviceSketch = sketch;
}

void GLWidget::recieveMouseMode(dureu::MOUSE_MODE mode)
{
    debugLogVal("Changing mouse mode to", mode);
    m_ModeMouse = mode;
    m_Manipulator->setMode(m_ModeMouse);
    m_EH->setMode(m_ModeMouse);
}

void GLWidget::initializeGL()
{
    osg::StateSet* stateSet = m_RootScene->getOrCreateStateSet();
    osg::Material* material = new osg::Material;
    material->setColorMode(osg::Material::AMBIENT_AND_DIFFUSE);
    stateSet->setAttributeAndModes(material, osg::StateAttribute::ON);
    stateSet->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);
}

void GLWidget::paintGL()
{
    this->m_Viewer->frame();
}

void GLWidget::resizeGL(int w, int h)
{
    this->getEventQueue()->windowResize(this->x(), this->y(), w, h);
    this->m_GraphicsWindow->resized(this->x(), this->y(), w, h);
    this->onResize(w, h);
}

void GLWidget::keyPressEvent(QKeyEvent *event)
{
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

void GLWidget::keyReleaseEvent(QKeyEvent *event)
{
    QString keystr = event->text();
    const char* keydat = keystr.toLocal8Bit().data();
    this->getEventQueue()->keyRelease(osgGA::GUIEventAdapter::KeySymbol(*keydat));
}

void GLWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (!m_DeviceDown || !m_DeviceActive) {
        this->getEventQueue()->mouseMotion(static_cast<float>(event->x()), static_cast<float>(event->y()));
    }
}

void GLWidget::mousePressEvent(QMouseEvent *event)
{
    if (!m_DeviceDown || !m_DeviceActive) {
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

void GLWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (!m_DeviceDown || !m_DeviceActive) {
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

void GLWidget::wheelEvent(QWheelEvent *event)
{
    event->accept();
    int delta = event->delta();
    osgGA::GUIEventAdapter::ScrollingMotion motion = delta > 0 ?   osgGA::GUIEventAdapter::SCROLL_UP
                                                                 : osgGA::GUIEventAdapter::SCROLL_DOWN;
    this->getEventQueue()->mouseScroll( motion );
}

void GLWidget::tabletEvent(QTabletEvent *event)
{
    event->accept();
    switch (event->type()){
    case QEvent::TabletPress:
        this->getEventQueue()->touchBegan(0, osgGA::GUIEventAdapter::TOUCH_BEGAN,
                                          static_cast<float>(event->x()), static_cast<float>(event->y()));
        break;
    case QEvent::TabletRelease:
        this->getEventQueue()->touchEnded(0, osgGA::GUIEventAdapter::TOUCH_ENDED,
                                          static_cast<float>(event->x()), static_cast<float>(event->y()), 1);
        break;
    case QEvent::TabletMove:
        this->getEventQueue()->touchMoved(0, osgGA::GUIEventAdapter::TOUCH_MOVED,
                                          static_cast<float>(event->x()), static_cast<float>(event->y()));
        break;
    default:
        break;
    }
}

bool GLWidget::event(QEvent *event)
{
    bool handled = QOpenGLWidget::event(event);
    switch(event->type()){
    case QEvent::TabletMove:
        this->update();
        break;
    case QEvent::TabletPress:
        m_DeviceDown = true;
        this->update();
        break;
    case QEvent::TabletRelease:
        this->update();
        m_DeviceDown = false;
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

void GLWidget::onHome()
{
    osgViewer::ViewerBase::Views views;
    m_Viewer->getViews(views);
    for (unsigned int i=0; i<views.size(); ++i){
        osgViewer::View* view = views.at(i);
        view->home();
    }
}

void GLWidget::onResize(int w, int h)
{
    std::vector<osg::Camera*> cameras;
    this->m_Viewer->getCameras(cameras);
    if (cameras.size() != this->m_ModeView) {
        debugErrMsg("onResize(): the cameras number does not correspond to view mode");
        debugLogVal("Camera size", cameras.size());
        debugLogVal("View mode", this->m_ModeView);
        return;
    }
    if (this->m_ModeView == 1)
        cameras[0]->setViewport( 0,0,this->width(), this->height() );
    else if (this->m_ModeView == 2){
        cameras[0]->setViewport( 0,0,this->width()/2, this->height() );
        cameras[1]->setViewport( this->width() / 2, 0, this->width() / 2, this->height() );
    }
    else if (this->m_ModeView == 3){
        cameras[0]->setViewport( 0,0,this->width()/3, this->height() );
        cameras[1]->setViewport( this->width() / 3, 0, this->width()/ 3, this->height() );
        cameras[2]->setViewport( this->width() * 2 / 3, 0, this->width()/ 3, this->height() );
    }
    else{
        debugErrMsg("onResize(): unsupported view mode");
        debugLogVal("Cameras number", cameras.size());
        return;
    }
}

osgGA::EventQueue *GLWidget::getEventQueue() const
{
    osgGA::EventQueue* eventQueue = m_GraphicsWindow->getEventQueue();
    if( eventQueue )
        return eventQueue;
    else
        throw std::runtime_error( "Unable to obtain valid event queue");
}

