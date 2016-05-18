#include <QApplication>
#include <QMainWindow>
#include <QOpenGLWidget>
#include <QMouseEvent>
#include <QWheelEvent>

#include <osg/ref_ptr>
#include <osgViewer/GraphicsWindow>
#include <osgViewer/Viewer>
#include <osg/Camera>
#include <osg/ShapeDrawable>
#include <osg/StateSet>
#include <osg/Material>
#include <osgGA/EventQueue>
#include <osgGA/TrackballManipulator>

#include <iostream>
#include <stdio.h>

class QtOSGWidget : public QOpenGLWidget
{
public:
  QtOSGWidget(QWidget* parent = 0)
      : QOpenGLWidget(parent)
        , _mGraphicsWindow(new osgViewer::GraphicsWindowEmbedded( this->x(), this->y(),
                                                                 this->width(), this->height() ) )
        , _mViewer(new osgViewer::Viewer)
      {
        osg::Cylinder* cylinder    = new osg::Cylinder( osg::Vec3( 0.f, 0.f, 0.f ), 0.25f, 0.5f );
        osg::ShapeDrawable* sd = new osg::ShapeDrawable( cylinder );
        sd->setColor( osg::Vec4( 0.8f, 0.5f, 0.2f, 1.f ) );
        osg::Geode* geode = new osg::Geode;
        geode->addDrawable(sd);

        osg::Camera* camera = new osg::Camera;
        camera->setViewport( 0, 0, this->width(), this->height() );
        camera->setClearColor( osg::Vec4( 0.9f, 0.9f, 1.f, 1.f ) );
        float aspectRatio = static_cast<float>( this->width()) / static_cast<float>( this->height() );
        camera->setProjectionMatrixAsPerspective( 30.f, aspectRatio, 1.f, 1000.f );
        camera->setGraphicsContext( _mGraphicsWindow );

        _mViewer->setCamera(camera);
        _mViewer->setSceneData(geode);
        osgGA::TrackballManipulator* manipulator = new osgGA::TrackballManipulator;
        manipulator->setAllowThrow( false );
        this->setMouseTracking(true);
        _mViewer->setCameraManipulator(manipulator);
        _mViewer->setThreadingModel(osgViewer::Viewer::SingleThreaded);
        _mViewer->realize();
      }


  virtual ~QtOSGWidget(){}

protected:

  virtual void paintGL() {
    _mViewer->frame();
  }

  virtual void resizeGL( int width, int height ) 
  {
      this->getEventQueue()->windowResize(this->x(), this->y(), width, height);
      _mGraphicsWindow->resized(this->x(), this->y(), width, height);
      osg::Camera* camera = _mViewer->getCamera();
      camera->setViewport(0, 0, this->width(), this->height());
  }

  virtual void initializeGL(){
      osg::Geode* geode = dynamic_cast<osg::Geode*>(_mViewer->getSceneData());
      osg::StateSet* stateSet = geode->getOrCreateStateSet();
      osg::Material* material = new osg::Material;
      material->setColorMode( osg::Material::AMBIENT_AND_DIFFUSE );
      stateSet->setAttributeAndModes( material, osg::StateAttribute::ON );
      stateSet->setMode( GL_DEPTH_TEST, osg::StateAttribute::ON );
  }

  virtual void mouseMoveEvent(QMouseEvent* event)
  {
      this->getEventQueue()->mouseMotion(event->x(), event->y());
  }

  virtual void mousePressEvent(QMouseEvent* event)
  {
      unsigned int button = 0;
      switch (event->button()){
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
      this->getEventQueue()->mouseButtonPress(event->x(), event->y(), button);
  }

  virtual void mouseReleaseEvent(QMouseEvent* event)
  {
      unsigned int button = 0;
      switch (event->button()){
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
      this->getEventQueue()->mouseButtonRelease(event->x(), event->y(), button);
  }

  virtual void wheelEvent(QWheelEvent* event)
  {
      int delta = event->delta();
      osgGA::GUIEventAdapter::ScrollingMotion motion = delta > 0 ?
                  osgGA::GUIEventAdapter::SCROLL_UP : osgGA::GUIEventAdapter::SCROLL_DOWN;
      this->getEventQueue()->mouseScroll(motion);
  }

  virtual bool event(QEvent* event)
  {
      bool handled = QOpenGLWidget::event(event);
      this->update();
      return handled;
  }

private:

  osgGA::EventQueue* getEventQueue() const {
    osgGA::EventQueue* eventQueue = _mGraphicsWindow->getEventQueue();
    return eventQueue;
  }

  osg::ref_ptr<osgViewer::GraphicsWindowEmbedded> _mGraphicsWindow;
  osg::ref_ptr<osgViewer::Viewer> _mViewer;
};

int main(int argc, char** argv){
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication qapp(argc, argv);
    QMainWindow window;
    QtOSGWidget* widget = new QtOSGWidget(&window);
    window.setCentralWidget(widget);
    window.show();

    return qapp.exec();
}
