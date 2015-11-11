#include <cassert>
#include <stdexcept>
#include <vector>
#include <iostream>
#include <string>

#include "osgwidget.h"
#include "pickhandler.h"

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
#include <osg/LineWidth>

#include <QDebug>
#include <QKeyEvent>
#include <QPainter>
#include <QWheelEvent>

osg::Drawable* createCanvas(const osg::Vec3& center, const osg::Vec3& w,const osg::Vec3& h){
    osg::ref_ptr<osg::Geometry> geom = new osg::Geometry;
    osg::Vec3Array* coords = new osg::Vec3Array(4);
    (*coords)[0] = center - w/2 - h/2;
    (*coords)[1] = center - w/2 + h/2;
    (*coords)[2] = center + w/2 - h/2;
    (*coords)[3] = center + w/2 + h/2;
    geom->setVertexArray(coords);
    osg::Vec4 border_clr(float(101)/255.0f,float(123)/255.0f,float(131)/255.0f,1.0f); // solarized base00

    osg::Vec4Array* color = new osg::Vec4Array(4);
    (*color)[0] = border_clr;
    (*color)[1] = border_clr;
    (*color)[2] = border_clr;
    (*color)[3] = border_clr;

    geom->setColorArray(color, osg::Array::BIND_PER_VERTEX);
    geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINES,0,4));

    osg::StateSet* stateset = new osg::StateSet;
    osg::LineWidth* linewidth = new osg::LineWidth();
    linewidth->setWidth(4.0f);
    stateset->setAttributeAndModes(linewidth,osg::StateAttribute::ON);
    stateset->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
    geom->setStateSet(stateset);

    return geom.release();
}

osg::Drawable* createAxis(const osg::Vec3& corner,const osg::Vec3& xdir,const osg::Vec3& ydir,const osg::Vec3& zdir)
{
    // set up the Geometry.
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

OSGWidget::OSGWidget(QWidget* parent, const int nview, const std::string& fname):
    QOpenGLWidget(parent),
    _graphicsWindow(new osgViewer::GraphicsWindowEmbedded(this->x(), this->y(), this->width(),this->height()) ),
    _viewer(new osgViewer::CompositeViewer),
    _selectionActive(false),
    _selectionFinished (true),
    _nview(nview),
    _tabletDevice(QTabletEvent::Stylus), // http://doc.qt.io/qt-5/qtabletevent.html#TabletDevice-enum
    _deviceDown(false),
    _deviceActive(false)
{
    osg::ref_ptr<osg::Group> root = new osg::Group;
    osg::ref_ptr<osg::Node> model = osgDB::readNodeFile(fname);
    osg::ref_ptr<osg::Geode> axes = new osg::Geode;
    osg::ref_ptr<osg::Geode> canva = new osg::Geode;
    axes->addDrawable(createAxis(osg::Vec3(0.0f,0.0f,0.0f), osg::Vec3(1.0f,0.0f,0.0f),
                                 osg::Vec3(0.0f,1.0f,0.0f), osg::Vec3(0.0f,0.0f,1.0f)));
    if (!fname.empty() && model!=NULL)
        root->addChild(model);
    else { // add a "canvas"
        canva->addDrawable(createCanvas(osg::Vec3(0.0f,0.0f,0.0f), osg::Vec3(5.0f,0.0f,0.0f),osg::Vec3(0.0f,3.0f,0.0f)));
        root->addChild(canva.get());
    }

    // Generated after the regualr scene is finished being drawn
    // on the screen. Set to the absolute reference frame, and
    // is set as a custom fixed view matrix.
    const int hudW=50;
    const int hudH=50;
    osg::ref_ptr<osg::Camera> _hudCamera = new osg::Camera;
    _hudCamera->setClearMask(GL_DEPTH_BUFFER_BIT);
    _hudCamera->setRenderOrder(osg::Camera::POST_RENDER);
    _hudCamera->setReferenceFrame(osg::Camera::ABSOLUTE_RF);
    _hudCamera->setAllowEventFocus(false);
    //hudCamera->setViewMatrixAsLookAt();
    _hudCamera->setViewport(0, this->height()-hudH, hudW, hudH);
    //_hudCamera->setProjectionMatrixAsPerspective( 30.f, aspectRatio, 1.f, 1000.f);
    _hudCamera->setClearColor(osg::Vec4(float(0)/255.0f, float(43)/255.0f,
                                        float(54)/255.0f, 1.0f)); // solarized base03
    _hudCamera->addChild(axes.get());
    root->addChild(_hudCamera.get());

    // Set material for basic lighting and enable depth tests. Or
    // will suffer from rendering errors.
    osg::StateSet* stateSet = root->getOrCreateStateSet();
    osg::Material* material = new osg::Material;
    material->setColorMode(osg::Material::AMBIENT_AND_DIFFUSE);
    stateSet->setAttributeAndModes(material, osg::StateAttribute::ON);
    stateSet->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);

    float aspectRatio = static_cast<float>( this->width() / _nview ) / static_cast<float>( this->height() );
    osg::Camera* camera = new osg::Camera;
    camera->setViewport( 0, 0, this->width() / _nview, this->height() );
    camera->setProjectionMatrixAsPerspective( 30.f, aspectRatio, 1.f, 1000.f );
    camera->setGraphicsContext( _graphicsWindow );
    camera->setClearColor(osg::Vec4(float(235)/255.0f, float(246)/255.0f,
                                    float(227)/255.0f, 1.0f)); // solarized base3

    osgGA::TrackballManipulator* manipulator = new osgGA::TrackballManipulator;
    manipulator->setAllowThrow( false );

    osgViewer::View* view = new osgViewer::View;
    view->setName("Single view");
    view->setCamera(camera);
    view->setSceneData(root.get());
    view->addEventHandler(new osgViewer::StatsHandler);
    //view->addEventHandler(new PickHandler);
    view->setCameraManipulator(manipulator);

    _viewer->addView(view);

    // code for side view
    if (_nview == 2){
        osg::Camera* sideCamera = new osg::Camera;
        sideCamera->setViewport(this->width()/_nview, 0, this->width()/_nview, this->height());
        sideCamera->setProjectionMatrixAsPerspective( 30.f, aspectRatio, 1.f, 1000.f );
        sideCamera->setGraphicsContext( _graphicsWindow );
        sideCamera->setClearColor(osg::Vec4(float(235)/255.0f, float(246)/255.0f,
                                        float(227)/255.0f, 1.0f)); // solarized base3

        osgGA::TrackballManipulator* sideManipulator = new osgGA::TrackballManipulator;
        sideManipulator->setAllowThrow( false );

        osgViewer::View* sideView = new osgViewer::View;
        sideView->setCamera( sideCamera );
        sideView->setSceneData( root.get() );
        sideView->addEventHandler( new osgViewer::StatsHandler );
        sideView->setCameraManipulator( sideManipulator );
        _viewer->addView(sideView);
    }

    _viewer->setThreadingModel(osgViewer::CompositeViewer::SingleThreaded);
    _viewer->realize();

    /// Widget will get the keyboard events. It is not set by default.
    this->setFocusPolicy(Qt::StrongFocus);
    this->setMinimumSize(512*_nview, 512);

    /// Widget will get mouse move events even with no mouse button pressed.
    /// It is required so that graphics window can switch viewports properly.
    this->setMouseTracking(true);
}

OSGWidget::~OSGWidget(){}

void OSGWidget::getTabletDevice(bool active){
    _deviceActive = active;
    std::cout << "device status: " << active << std::endl;
}

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
    _graphicsWindow->resized(this->x(), this->y(), w, h);
    this->onResize(w, h);
}

/// Does keyboard press processing, for example:
/// key "s" is for selection mode (draws rectangles)
/// key "d" is for saving the scene data
/// key "h" is for going to "home" camera setting
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
    else if (!_deviceDown || !_deviceActive) {
        this->getEventQueue()->mouseMotion(static_cast<float>(event->x()), static_cast<float>(event->y()));
    }
}

void OSGWidget::mousePressEvent( QMouseEvent* event )
{
    // Selection processing
    if( _selectionActive && event->button() == Qt::LeftButton ) {
        _selectionStart    = event->pos();
        _selectionEnd      = _selectionStart; // Deletes the old selection
        _selectionFinished = false;           // As long as this is set, the rectangle will be drawn
    }
    else if (!_deviceDown || !_deviceActive) {
        std::cout << "mouse pressed" << std::endl;
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

void OSGWidget::mouseReleaseEvent(QMouseEvent* event) {
    // Selection processing: Store end position and obtain selected objects
    // through polytope intersection.
    if( _selectionActive && event->button() == Qt::LeftButton ) {
        _selectionEnd      = event->pos();
        _selectionFinished = true; // Will force the painter to stop drawing the selection rectangle
        this->processSelection();
    }
    else if (!_deviceDown || !_deviceActive) {
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

void OSGWidget::tabletEvent(QTabletEvent *event){
    unsigned int button = 0;
    int osg_rotate = 1, osg_pan = 2, osg_zoom = 3;
    switch (event->type()){
    case QEvent::TabletPress:
        std::cout << std::endl << "Tablet pressed" << std::endl;
        //if (!_deviceDown) { }
        switch(event->buttons()) {
        ///case 1: // touch and no buttons are pressed, default: rotate
        case 2: // no touch and lower button pressed, default: zoom
            button = osg_rotate;
            break;
        case 3: // touch and lower button pressed, default: pan
            button = osg_rotate;
            break;
        case 4: // no touch and upper button pressed, default: rotate
            button = osg_pan;
            break;
        case 5: // touch and upper button pressed, default: rotate
            button = osg_pan;
            break;
        default:
            break;
        }
        this->getEventQueue()->mouseButtonPress(static_cast<float>(event->x()), static_cast<float>(event->y()), button);
        break;
    case QEvent::TabletRelease:
        std::cout << "Tablet released" << std::endl << std::endl;
        //if (_deviceDown){ }
        switch(event->button()) { // buttons() will not work here, so have to use button()
        /// 1: no button released, whether in the air or in touch
        /// 2: lower button released, whether in the air or in touch
        /// 4: top button released, whether in the air or in touch
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
        //if (_deviceDown){ }
        this->getEventQueue()->mouseMotion(static_cast<float>(event->x()), static_cast<float>(event->y()));
        break;
    }
}

/// A general function for any event. It ensures that OSGWidget is always repainted
/// after the user's interactions. It is put into an event handler for ensuring to
/// include all the possible events, and also for preventing code duplicates.
/// The tablet events are sent to tabletEvent() if the device is used (this
/// could be processed in MainWindow?).
bool OSGWidget::event( QEvent* event ) {
    bool handled = QOpenGLWidget::event( event );
    switch( event->type() ) {
    case QEvent::TabletMove:
        event->accept();
        //tabletEvent(static_cast<QTabletEvent*>(event));
        this->update();
        break;
    case QEvent::TabletPress:
        event->accept();
        _deviceDown = true;
        //tabletEvent(static_cast<QTabletEvent*>(event));
        this->update();
        break;
    case QEvent::TabletRelease:
        event->accept();
        //tabletEvent(static_cast<QTabletEvent*>(event));
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

/// Return to the very initial "home" view.
void OSGWidget::onHome() {
    osgViewer::ViewerBase::Views views;
    _viewer->getViews( views );
    for( std::size_t i = 0; i < views.size(); i++ ) {
        osgViewer::View* view = views.at(i);
        view->home();
    }
}

void OSGWidget::onResize( int w, int h ) {
    std::vector<osg::Camera*> cameras;
    _viewer->getCameras( cameras );
    assert( cameras.size() == _nview);
    //cameras[0]->setViewport( 0, 0, this->width(), this->height() );
    cameras[0]->setViewport( 0, 0, this->width() / _nview, this->height() );
    if (_nview == 2)
        cameras[1]->setViewport( this->width() / _nview, 0, this->width() / _nview, this->height() );
}

osgGA::EventQueue* OSGWidget::getEventQueue() const {
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

        //auto intersections = polytopeIntersector->getIntersections();

        // c++11 only
        //for( auto&& intersection : intersections )
        //    qDebug() << "Selected a drawable:" << QString::fromStdString( intersection.drawable->getName() );
    }
}
