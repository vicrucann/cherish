#ifndef VIEWWIDGET
#define VIEWWIDGET

/* ViewWidget class
 * This is a connecting class between Qt and OSG.
 * Qt collects the user input by means of mouse, keyboard and
 * Wacom tablet.
 * OSG captures those events and does corresponding scene graph
 * manipulation or editing.
 */

#include <QOpenGLWidget>
#include <QTabletEvent>

#include <osg/ref_ptr>
#include <osgViewer/GraphicsWindow>
#include <osgViewer/CompositeViewer>
#include "rootscene.h"

class ViewWidget : public QOpenGLWidget {
    Q_OBJECT
public:
    ViewWidget(osg::ref_ptr<RootScene>& root, QWidget* parent=0, Qt::WindowFlags f = 0, int viewmode=1);
    virtual ~ViewWidget();
public slots:
    void getTabletActivity(bool active);
    void getStylusSketchStatus(bool sketch);
protected:
    void paintEvent(QPaintEvent* pev) Q_DECL_OVERRIDE;
    void paintGL() Q_DECL_OVERRIDE;
    void resizeGL(int w, int h) Q_DECL_OVERRIDE;

    virtual void keyPressEvent(QKeyEvent* event);
    virtual void keyReleaseEvent(QKeyEvent* event);

    virtual void mouseMoveEvent(QMouseEvent* event);
    virtual void mousePressEvent(QMouseEvent* event);
    virtual void mouseReleaseEvent(QMouseEvent* event);
    virtual void wheelEvent(QWheelEvent* event);

    virtual bool event(QEvent* event);

    void tabletEvent(QTabletEvent* event) Q_DECL_OVERRIDE;
private:
    virtual void onHome();
    virtual void onResize(int w, int h);

    osgGA::EventQueue* getEventQueue() const; // for osg to process mouse and keyboard events
    // for more info see reference osgGA::EventQueue and osgGA::GUIEventAdapter
    // the later's enums are used in EventHandler.h

    osg::ref_ptr<osgViewer::GraphicsWindowEmbedded> _graphicsWindow; // osg graphics context
    osg::ref_ptr<osgViewer::CompositeViewer> _viewer; // Camera setup
    osg::ref_ptr<RootScene>& _root; // scene to view

    QTabletEvent::TabletDevice _tabletDevice;

    int _viewmode; // number of views per CompositeViewer
    bool _deviceDown; // pen touches the device?
    bool _deviceActive; // pen is in device approximation?
    bool _deviceSketch;
};

#endif // VIEWWIDGET

