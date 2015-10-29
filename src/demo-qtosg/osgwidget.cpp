#include <cassert>
#include <stdexcept>
#include <vector>

#include "osgwidget.h"

#include <osg/Camera>
#include <osg/StateSet>
#include <osg/Material>
#include <osgDB/ReadFile>
#include <osgDB/WriteFile>
#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>
#include <osgGA/EventQueue>
#include <osgGA/TrackballManipulator>
#include <osgUtil/IntersectionVisitor>
#include <osgUtil/PolytopeIntersector>

#include <QDebug>
#include <QKeyEvent>
#include <QPainter>
#include <QWheelEvent>



OSGWidget::OSGWidget(QWidget* parent):
    QOpenGLWidget(parent),
    _graphicsWindow(new osgViewer::GraphicsWindowEmbedded(this->x(), this->y(), this->width(),this->height()) ),
    //_graphicsWindow(new osgViewer::GraphicsWindow()),
    _viewer(new osgViewer::CompositeViewer),
    _selectionActive(false),
    _selectionFinished (true)
{
    osg::ref_ptr<osg::Node> root = osgDB::readNodeFile("../demo-osg/cow.osgt");

    // Set material for basic lighting and enable depth tests. Else, the sphere
    // will suffer from rendering errors.
    osg::StateSet* stateSet = root->getOrCreateStateSet();
    osg::Material* material = new osg::Material;
    material->setColorMode(osg::Material::AMBIENT_AND_DIFFUSE);
    stateSet->setAttributeAndModes(material, osg::StateAttribute::ON);
    stateSet->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);

    float aspectRatio = static_cast<float>( this->width() / 2 ) / static_cast<float>( this->height() );
    osg::Camera* camera = new osg::Camera;
    camera->setViewport( 0, 0, this->width() / 2, this->height() );
    camera->setClearColor( osg::Vec4( 0.f, 0.f, 1.f, 1.f ) );
    camera->setProjectionMatrixAsPerspective( 30.f, aspectRatio, 1.f, 1000.f );
    camera->setGraphicsContext( _graphicsWindow );

    osgViewer::View* view = new osgViewer::View;
    view->setCamera(camera);
    view->setSceneData(root.get());
    view->addEventHandler(new osgViewer::StatsHandler);
    //view->addEventHandler(new PickHandler);

    osgGA::TrackballManipulator* manipulator = new osgGA::TrackballManipulator;
    manipulator->setAllowThrow( false );

    view->setCameraManipulator(manipulator);

    // code for side view
    osg::Camera* sideCamera = new osg::Camera;
    sideCamera->setViewport(this->width()/2, 0, this->width()/2, this->height());
    sideCamera->setClearColor( osg::Vec4( 0.f, 0.f, 1.f, 1.f ) );
    sideCamera->setProjectionMatrixAsPerspective( 30.f, aspectRatio, 1.f, 1000.f );
    sideCamera->setGraphicsContext( _graphicsWindow );

    osgViewer::View* sideView = new osgViewer::View;
    sideView->setCamera( sideCamera );
    sideView->setSceneData( root.get() );
    sideView->addEventHandler( new osgViewer::StatsHandler );
    sideView->setCameraManipulator( new osgGA::TrackballManipulator );

    _viewer->addView(view);
    _viewer->addView(sideView);
    _viewer->setThreadingModel(osgViewer::CompositeViewer::SingleThreaded);
    _viewer->realize();

    // Widget will get the keyboard events
    // It is not set by default
    this->setFocusPolicy(Qt::StrongFocus);
    this->setMinimumSize(512, 512);

    // Widget will get mouse move events even
    // with no mouse button pressed
    // It is required so that graphics window
    // can switch viewports properly
    this->setMouseTracking(true);
}

OSGWidget::~OSGWidget(){}

/// relative to the first point, the second point may be in
/// either one of the four quadrants of an Euclidian coordinate
/// system. They are enumerated in counter-clockwise order,
/// starting from the lower-right quadrant that corresponds to
/// the default case.
QRect makeRectangle(const QPoint& first, const QPoint& second){
    if( second.x() >= first.x() && second.y() >= first.y() )
        return QRect( first, second );
    else if( second.x() < first.x() && second.y() >= first.y() )
        return QRect( QPoint( second.x(), first.y() ), QPoint( first.x(), second.y() ) );
    else if( second.x() < first.x() && second.y() < first.y() )
        return QRect( second, first );
    else if( second.x() >= first.x() && second.y() < first.y() )
        return QRect( QPoint( first.x(), second.y() ), QPoint( second.x(), first.y() ) );

    // should never reach this point
    return QRect();
}

void OSGWidget::paintEvent(QPaintEvent *pev){
    this->makeCurrent();
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    this->paintGL();
    if (_selectionActive && !_selectionFinished){
        painter.setPen( Qt::black );
        painter.setBrush( Qt::transparent );
        painter.drawRect(makeRectangle(_selectionStart, _selectionEnd));
    }
    painter.end();
    this->doneCurrent();
}

void OSGWidget::paintGL(){
    _viewer->frame();
}

void OSGWidget::resizeGL(int w, int h){
    this->getEventQueue()->windowResize(this->x(), this->y(), w, h);
    //this->resize(QSize(w,h));
    _graphicsWindow->resized(this->x(), this->y(), w, h);
    this->onResize(w, h);
}

/// Does keyboard press processing, for example:
/// key "s" is for selection
/// key "d" is for saving the scene data
/// key "h" is for going to "home" setting
void OSGWidget::keyPressEvent(QKeyEvent *event){
    QString keystr = event->text();
    const char* keydat = keystr.toLocal8Bit().data();

    if (event->key() == Qt::Key_S) {
        _selectionActive = !_selectionActive;
    }
    else if (event->key() == Qt::Key_D) {
        osgDB::writeNodeFile(*_viewer->getView(0)->getSceneData(), "tmp/sg.osg");
        return;
    }
    else if (event->key() == Qt::Key_H) {
        this->onHome();
        return;
    }
    this->getEventQueue()->keyPress(osgGA::GUIEventAdapter::KeySymbol (*keydat));
}

void OSGWidget::keyReleaseEvent(QKeyEvent *event){
    QString keystr = event->text();
    const char* keydat = keystr.toLocal8Bit().data();
    this->getEventQueue()->keyRelease(osgGA::GUIEventAdapter::KeySymbol(*keydat));
}

void OSGWidget::mouseMoveEvent(QMouseEvent *event){
    if (_selectionActive && event->buttons() & Qt::LeftButton){
        _selectionEnd = event->pos();
        this->update();
    }
    else {
        this->getEventQueue()->mouseMotion(static_cast<float>(event->x()),
                                           static_cast<float>(event->y()));
    }
}

void OSGWidget::mousePressEvent( QMouseEvent* event )
{
    // Selection processing
    if( _selectionActive && event->button() == Qt::LeftButton )
    {
        _selectionStart    = event->pos();
        _selectionEnd      = _selectionStart; // Deletes the old selection
        _selectionFinished = false;           // As long as this is set, the rectangle will be drawn
    }

    // Normal processing
    else
    {
        // 1 = left mouse button
        // 2 = middle mouse button
        // 3 = right mouse button

        unsigned int button = 0;

        switch( event->button() )
        {
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

        this->getEventQueue()->mouseButtonPress( static_cast<float>( event->x() ),
                                                 static_cast<float>( event->y() ),
                                                 button );
    }
}

void OSGWidget::mouseReleaseEvent(QMouseEvent* event)
{
    // Selection processing: Store end position and obtain selected objects
    // through polytope intersection.
    if( _selectionActive && event->button() == Qt::LeftButton )
    {
        _selectionEnd      = event->pos();
        _selectionFinished = true; // Will force the painter to stop drawing the
        // selection rectangle

        this->processSelection();
    }

    // Normal processing
    else
    {
        // 1 = left mouse button
        // 2 = middle mouse button
        // 3 = right mouse button

        unsigned int button = 0;

        switch( event->button() )
        {
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

        this->getEventQueue()->mouseButtonRelease( static_cast<float>( event->x() ),
                                                   static_cast<float>( event->y() ),
                                                   button );
    }
}

void OSGWidget::wheelEvent( QWheelEvent* event )
{
    // Ignore wheel events as long as the selection is active.
    if( _selectionActive )
        return;

    event->accept();
    int delta = event->delta();

    osgGA::GUIEventAdapter::ScrollingMotion motion = delta > 0 ?   osgGA::GUIEventAdapter::SCROLL_UP
                                                                 : osgGA::GUIEventAdapter::SCROLL_DOWN;

    this->getEventQueue()->mouseScroll( motion );
}

bool OSGWidget::event( QEvent* event )
{
    bool handled = QOpenGLWidget::event( event );

    // This ensures that the OSG widget is always going to be repainted after the
    // user performed some interaction. Doing this in the event handler ensures
    // that we don't forget about some event and prevents duplicate code.
    switch( event->type() )
    {
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

void OSGWidget::onHome()
{
    osgViewer::ViewerBase::Views views;
    _viewer->getViews( views );

    for( std::size_t i = 0; i < views.size(); i++ )
    {
        osgViewer::View* view = views.at(i);
        view->home();
    }
}

void OSGWidget::onResize( int w, int h )
{
    std::vector<osg::Camera*> cameras;
    _viewer->getCameras( cameras );

    assert( cameras.size() == 2 );

    cameras[0]->setViewport( 0, 0, this->width() / 2, this->height() );
    cameras[1]->setViewport( this->width() / 2, 0, this->width() / 2, this->height() );
}

osgGA::EventQueue* OSGWidget::getEventQueue() const
{
    osgGA::EventQueue* eventQueue = _graphicsWindow->getEventQueue();

    if( eventQueue )
        return eventQueue;
    else
        throw std::runtime_error( "Unable to obtain valid event queue");
}

void OSGWidget::processSelection(){
    QRect selectionRectangle = makeRectangle( _selectionStart, _selectionEnd );
    int widgetHeight = this->height();
    double xMin = selectionRectangle.left();
    double xMax = selectionRectangle.right();
    double yMin = widgetHeight - selectionRectangle.bottom();
    double yMax = widgetHeight - selectionRectangle.top();

    osgUtil::PolytopeIntersector* polytopeIntersector
            = new osgUtil::PolytopeIntersector( osgUtil::PolytopeIntersector::WINDOW,
                                                xMin, yMin,
                                                xMax, yMax );
    // This limits the amount of intersections that are reported by the
    // polytope intersector. Using this setting, a single drawable will
    // appear at most once while calculating intersections. This is the
    // preferred and expected behaviour.
    polytopeIntersector->setIntersectionLimit( osgUtil::Intersector::LIMIT_ONE_PER_DRAWABLE );
    osgUtil::IntersectionVisitor iv( polytopeIntersector );

    for( unsigned int viewIndex = 0; viewIndex < _viewer->getNumViews(); viewIndex++ )
    {
        osgViewer::View* view = _viewer->getView( viewIndex );
        if( !view )
            throw std::runtime_error( "Unable to obtain valid view for selection processing" );

        osg::Camera* camera = view->getCamera();
        if( !camera )
            throw std::runtime_error( "Unable to obtain valid camera for selection processing" );
        camera->accept( iv );

        if( !polytopeIntersector->containsIntersections() )
            continue;

        auto intersections = polytopeIntersector->getIntersections();

        // c++11 only
        //for( auto&& intersection : intersections )
        //    qDebug() << "Selected a drawable:" << QString::fromStdString( intersection.drawable->getName() );
    }

}
