#ifndef OSGWIDGET
#define OSGWIDGET

/* OSGWidget
 * A widget that inherits from QOpenGLWidget for encapsulating the rendering.
 * The render is done only on demand, so no QTimer is used.
 * Does not require any threaded redraw functionalities.
 * Relies on builtin event system by Qt.
 * The widget contains an instance osgViewer::GraphicsWindowEmbedded,
 * Whose graphics context could be attached to any number of cameras.
 * The cameras could be attached to some osgViewer::CompositeViewer instance.
 *
 * The class requires minimum of: Qt 5.4, CMake 2.8.11, OpenSceneGraph 3.2
 *
 * Some usefule links:
 * The class is built based on the article found at
 * <http://bastian.rieck.ru/blog/posts/2014/qt_and_openscenegraph/>
 * To extend the OSGWidget for threaded OpenGL:
 * <http://blog.qt.io/blog/2011/06/03/threaded-opengl-in-4-8/>.
 * The incorporation of Wacom tablet pen events is based on:
 * <http://doc.qt.io/qt-5/qtwidgets-widgets-tablet-example.html>.
 * All accessed Oct 2015.
 *
 * Victoria Rudakova, Yale Graphics, 2015
 * <victoria.rudakova@yale.edu>
*/

#include <QOpenGLWidget>
#include <QTabletEvent>

#include <osg/ref_ptr>
#include <osgViewer/GraphicsWindow>
#include <osgViewer/CompositeViewer>

class OSGWidget : public QOpenGLWidget {
    Q_OBJECT
public:
    OSGWidget(QWidget* parent = 0);
    virtual ~OSGWidget();

    virtual void setTabletDevice(QTabletEvent::TabletDevice device) { _device = device; }
protected:
    virtual void paintEvent(QPaintEvent* pev) Q_DECL_OVERRIDE;
    virtual void paintGL() Q_DECL_OVERRIDE;
    virtual void resizeGL(int w, int h) Q_DECL_OVERRIDE;

    virtual void keyPressEvent(QKeyEvent* kev);
    virtual void keyReleaseEvent(QKeyEvent* kev);

    virtual void mouseMoveEvent(QMouseEvent* mev);
    virtual void mousePressEvent(QMouseEvent* mev);
    virtual void wheelEvent(QWheelEvent* wev);

    virtual bool event(QEvent* ev);

    virtual void tabletEvent(QTabletEvent* ev) Q_DECL_OVERRIDE;

private:
    virtual void onResize(int w, int h);

    osg::ref_ptr<osg::Viewer::CompositeViewer> _viewer;

    QTabletEvent::PointerType _pointer;
    QTabletEvent::TabletDevice _device;

}; // class OSGWidget


#endif // OSGWIDGET

