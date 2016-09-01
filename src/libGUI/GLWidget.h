#ifndef GLWIDGET
#define GLWIDGET

#include <QOpenGLWidget>
#include <QTabletEvent>
#include <QUndoStack>
#include <QStack>
#include <QPixmap>
#include <QScopedPointer>
#include <QDragEnterEvent>
#include <QDragLeaveEvent>
#include <QDragMoveEvent>
#include <QDropEvent>

#include <osg/ref_ptr>
#include <osg/observer_ptr>
#include <osgViewer/GraphicsWindow>
#include <osgViewer/CompositeViewer>
#include <osgViewer/Viewer>

#include "RootScene.h"
#include "Settings.h"
#include "../libSGControls/Manipulator.h"
#include "../libSGControls/EventHandler.h"
#include "../libSGControls/ViewerCommand.h"

class EventHandler;

/*! \class GLWidget
 * \brief A proxy class connecting QOpenGLWidget with the content of RootScene; and also to transmit events.
*/
class GLWidget : public QOpenGLWidget {
    Q_OBJECT
public:
    /*! Construtor. */
    GLWidget(RootScene* root, QUndoStack* stack, QWidget* parent=0, Qt::WindowFlags f = 0);

    /*! \return a non-const pointer on osg::Camera that is current for the GLWidget. */
    osg::Camera* getCamera() const;

protected:
    void setCameraView();
public:
    /*! Method to set up the manipulator to specified camera paramenters. */
    void setCameraView(const osg::Vec3d& eye, const osg::Vec3d& center, const osg::Vec3d& up, const double& fov);

    /*! Method to return manipulator's camera parameters. */
    void getCameraView(osg::Vec3d& eye, osg::Vec3d& center, osg::Vec3d& up, double& fov) const;

    /*! \param mode is the mouse mode to set up for the GLWidget. */
    void setMouseMode(const cher::MOUSE_MODE& mode);

signals:
    /*! Signal is emitted on double click to automatically switch the mouse mode. */
    void autoSwitchMode(cher::MOUSE_MODE mode);

    /*! Signal is emitted when mouse mode was set. */
    void mouseModeSet(cher::MOUSE_MODE mode);

    /*! Signal is emitted when FOV was changed. */
    void FOVSet(double fov);

    /*! Signal is emitted when user performs drag-and-drop from PhotoWidget to GLWidget. */
    void importPhoto(const QString& path, const QString& fileName);

public:
    /*! Method to set tablet proximity flag. */
    void setTabletActivity(bool active);

public slots:
    /*! Slot to obtain a scene graph screenshot by given canera position.  */
    void onRequestScreenshot(QPixmap& pmap, const osg::Vec3d& eye, const osg::Vec3d& center, const osg::Vec3d& up);

    /*! \param fov is the new  FOV (to change manipulator's camera) */
    void onFOVChangedSlider(double fov);

    /*! \param ortho - true of "ortho" mode is set, false - otherwise. */
    void onOrthoSet(bool ortho);

protected:
    // Widget's events that need to be over-ridden
    virtual void initializeGL();
    virtual void paintGL();
    virtual void resizeGL(int w, int h);

    virtual void keyPressEvent(QKeyEvent* event);
    virtual void keyReleaseEvent(QKeyEvent* event);

    virtual void mouseMoveEvent(QMouseEvent* event);
    virtual void mousePressEvent(QMouseEvent* event);
    virtual void mouseDoubleClickEvent(QMouseEvent* event);
    virtual void mouseReleaseEvent(QMouseEvent* event);
    virtual void wheelEvent(QWheelEvent* event);
    virtual void tabletEvent(QTabletEvent* event);

    virtual bool event(QEvent* event);

protected:
    // Drag and drop related (from PhotoWidget)
    void dragEnterEvent(QDragEnterEvent* event);
    void dragLeaveEvent(QDragLeaveEvent* event);
    void dragMoveEvent(QDragMoveEvent* event);
    void dropEvent(QDropEvent* event);

private:
    virtual void onHome();
    virtual void onResize(int w, int h);

    osgGA::EventQueue* getEventQueue() const; // for osg to process mouse and keyboard events
    // for more info see reference osgGA::EventQueue and osgGA::GUIEventAdapter
    // the later's enums are used in EventHandler.h

    osg::ref_ptr<osg::GraphicsContext::Traits> m_traits;
    osg::ref_ptr<osgViewer::GraphicsWindowEmbedded> m_graphicsWindow;
    osg::ref_ptr<osgViewer::Viewer> m_viewer;
    osg::observer_ptr<RootScene> m_RootScene;

    QTabletEvent::TabletDevice m_TabletDevice;

    bool m_DeviceDown; // pen touches the device?
    bool m_DeviceActive; // pen is in device approximation?

    cher::MOUSE_MODE m_mouseMode;
    osg::ref_ptr<Manipulator> m_manipulator;
    osg::ref_ptr<EventHandler> m_EH;

    //QStack<osg::Matrixd> m_stackView;

    QUndoStack* m_viewStack;
    osg::Vec3d m_eye, m_center, m_up; /* for prev/next views */
};

#endif // GLWIDGET

