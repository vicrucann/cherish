#ifndef VIEWWIDGET
#define VIEWWIDGET

#include <QOpenGLWidget>
#include <QTabletEvent>

#include <osg/ref_ptr>
#include <osgViewer/GraphicsWindow>
#include <osgViewer/CompositeViewer>

class ViewWidget : public QOpenGLWidget {
    Q_OBJECT
public:
    ViewWidget(QWidget* parent=0, int viewmode=1);
    virtual ~ViewWidget();
public slots:
    void getTabletActivity(bool active);
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

    osgGA::EventQueue* getEventQueue() const;

    osg::ref_ptr<osgViewer::GraphicsWindowEmbedded> _graphicsWindow;
    osg::ref_ptr<osgViewer::CompositeViewer> _viewer;

    QTabletEvent::TabletDevice _tabletDevice;

    int _viewmode; // number of views per CompositeViewer
    bool _deviceDown; // pen touches the device?
    bool _deviceActive; // pen is in device approximation?

};

#endif // VIEWWIDGET

