#ifndef BASEHANDLER
#define BASEHANDLER

/* BaseHandler
 * This is the base event handler class for all the user-defined
 * handlers. Its main characteristic is the usage of the middle
 * mouse button.
 * Middle mouse button press and drag: rotate the camera view.
 * Middle mouse button scroll: zoom-in and zoom-out.
 * The middle mouse button functionality will be present for any
 * other user-defined handlers, EXCEPT: in FixedViewHandler where
 * the rotation is overriden (replaced by zoom).
 * For more details on OSG GUIEventHandler: see p. 232-234, Open
 * Scene Graph: beginner's guide
 * The base class is GUIEventHandler and not StandardManipulator or
 * similar because we want to be able to process enum EventType which
 * includes not only mouse and keyboard, but also Wacom pen events.
 */

#include <osgGA/GUIEventHandler>
#include <osgGA/GUIEventAdapter>
#include <osgGA/GUIActionAdapter>
#include <osgGA/OrbitManipulator>
#include <osg/ref_ptr>

#include "rootscene.h"

class BaseHandler : public osgGA::GUIEventHandler {
public:
    BaseHandler();
    virtual bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);
protected:
    virtual void pan(float x, float y);
    virtual void zoomMouse(float mouseY, int widnowHeight);
    virtual void zoomScroll(float dy);
    virtual void rotate(float x, float y);

    //virtual void zoomIn();
    //virtual void zoomOut();
    //virtual void rotate(float mouseXNorm, float mouseYNorm);
    void setWheelZoomFactor(double wzf);
    void setTrackballSize(double ts);
private:
    void adjustCamera(osg::BoundingSphere bs);
    void trackball(osg::Quat &rotq, float x, float y, float dx, float dy);
    float tb_project_to_sphere( float r, float x, float y );

    double _zoom;
    float _panX, _panY;
    float _angleH, _angleV; // horizontal and vertical angles of rotation
    float _dx, _dy; // mouse coords on screen
    float _nx, _ny; // to keep normalized screen coords
    osg::Quat _rotation;
    osg::Vec3d _eye, _center, _up; // camera params

    double _wheelZoomFactor;
    double _trackballSize;
    osg::ref_ptr<osgGA::OrbitManipulator> _orbit;
};



#endif // BASEHANDLER

