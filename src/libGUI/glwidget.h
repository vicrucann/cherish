#ifndef GLWIDGET
#define GLWIDGET

#include <QOpenGLWidget>
#include <QTabletEvent>

#include <osg/ref_ptr>
#include <osg/observer_ptr>
#include <osgViewer/GraphicsWindow>
#include <osgViewer/CompositeViewer>
#include <osgViewer/Viewer>

#include "rootscene.h"
#include "settings.h"
#include "../libSGControls/manipulator.h"
#include "../libSGControls/EventHandler.h"

class GLWidget : public QOpenGLWidget {
    Q_OBJECT
public:
    GLWidget(RootScene* root, QWidget* parent=0, Qt::WindowFlags f = 0);
    virtual ~GLWidget();

public slots:
    void getTabletActivity(bool active);
    void getStylusSketchStatus(bool sketch);
    void recieveMouseMode(dureu::MOUSE_MODE mode);

protected:
    // Widget's events that need to be over-ridden
    virtual void initializeGL();
    virtual void paintGL();
    virtual void resizeGL(int w, int h);

    virtual void keyPressEvent(QKeyEvent* event);
    virtual void keyReleaseEvent(QKeyEvent* event);

    virtual void mouseMoveEvent(QMouseEvent* event);
    virtual void mousePressEvent(QMouseEvent* event);
    virtual void mouseReleaseEvent(QMouseEvent* event);
    virtual void wheelEvent(QWheelEvent* event);
    virtual void tabletEvent(QTabletEvent* event);

    virtual bool event(QEvent* event);

private:
    virtual void onHome();
    virtual void onResize(int w, int h);

    osgGA::EventQueue* getEventQueue() const; // for osg to process mouse and keyboard events
    // for more info see reference osgGA::EventQueue and osgGA::GUIEventAdapter
    // the later's enums are used in EventHandler.h

    osg::ref_ptr<osgViewer::GraphicsWindowEmbedded> m_GraphicsWindow;
    osg::ref_ptr<osgViewer::CompositeViewer> m_Viewer;
    osg::observer_ptr<RootScene> m_RootScene;

    QTabletEvent::TabletDevice m_TabletDevice;

    int m_ModeView; // number of views per CompositeViewer
    bool m_DeviceDown; // pen touches the device?
    bool m_DeviceActive; // pen is in device approximation?
    bool m_DeviceSketch; // may be obsolete variable

    dureu::MOUSE_MODE m_ModeMouse;
    osg::ref_ptr<Manipulator> m_Manipulator;
    osg::ref_ptr<EventHandler> m_EH;
};

#endif // GLWIDGET

